#include "filemanager.h"
#include <QCoreApplication>
#include <QDir>


FileManager::FileManager(QObject* parent, QString path_to_dir)
    : QObject(parent)
{
    // Если путь в конце не содержит /, то необходимо его добавить
    if(path_to_dir.endsWith("/")){
        localfilesPath = QCoreApplication::applicationDirPath() + path_to_dir;
    }
    else{
        localfilesPath = QCoreApplication::applicationDirPath() + path_to_dir + "/";
    }

    // Восстанавливаем на основе файлов map
    foreach (auto file, QDir(localfilesPath).entryList(QDir::Files)) {
        QFileInfo fileInfo(file);
        QString baseName = fileInfo.completeBaseName();
        QString suffix = fileInfo.suffix();

        // Пропускаем служебные .txt файлы (папки)
        if (suffix == "txt") {
            localFolders.insert(baseName, new QFile(localfilesPath + file));
        } else {
            // Парсим имя: "Grafy.docx" -> name="Grafy", ext="docx"
            QStringList parts = baseName.split('.');
            if (parts.size() >= 2) {
                QString name = parts.first();
                QString ext = parts.last().toLower();
                if (ext == "docx" || ext == "doc" || ext == "md" || ext == "markdown" || ext == "html") {
                    localFiles.insert(name, localfilesPath + file);
                    localFileTypes.insert(name, ext);
                }
            }
        }
    }
}

QStringList FileManager::add_files(QStringList files)
{
    QStringList added_files = QStringList();
    // Все указанные файлы конвертируем и добавляем в папку с файлами проекта
    foreach (auto filePath, files) {
        QString name = QFileInfo(filePath).baseName();
        QString ext = QFileInfo(filePath).suffix().toLower();
        // Если файла с таким именем ещё нет, то добавляем его в дерево
        if(!localFiles.contains(name)){
            QString destName = name + "." + ext + ".html";  // Сохраняем как HTML
            // Путь к директории с файлами(внутри проекта) + имя данного файла с расширением
            QString dest = localfilesPath + destName;

            if (ext == "docx" || ext == "doc" || ext == "md" || ext == "markdown") {
                QString htmlContent;
                if (::loadDocument(filePath, htmlContent, getPandocPath())) {
                    // Сохраняем HTML
                    QFile outFile(dest);
                    if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream out(&outFile);
                        out << htmlContent;
                        outFile.close();
                        localFiles.insert(name, dest);
                        localFileTypes.insert(name, ext);
                        added_files.append(name);
                    }
                }
            } else if (ext == "html") {
                // Просто копируем HTML
                if (QFile::copy(filePath, dest)) {
                    localFiles.insert(name, dest);
                    localFileTypes.insert(name, ext);
                    added_files.append(name);
                }
            }
        }
    }
    return added_files;
}

void FileManager::create_files(QStringList file_names)
{
    foreach (auto file, file_names) {
        QString full_name = file + ".html";

        // Если файла с таким именем ещё нет, то добавляем его в дерево
        if(!localFiles.contains(file)){
            // Путь к директории с файлами(внутри проекта) + имя данного файла с расширением
            QString dest = localfilesPath + full_name;

            QFile new_file(dest);
            if(new_file.open(QIODeviceBase::WriteOnly)){
                // Добавляем в map локальный файл
                localFiles.insert(file, dest);
            }
        }
    }
}

void FileManager::write_to_file(const QString& file_name, const QString& content)
{
    QFile target(localFiles.value(file_name));

    if(target.open(QIODeviceBase::WriteOnly | QIODeviceBase::Text)){
        QTextStream write(&target);
        write << content;  // Записываем содержимое
        target.close();     // Закрываем файл
    }
    else {
        qWarning() << "Cannot open file for writing:" << target.fileName();
    }
    emit fileSaved(file_name);
    emit modificationChanged(false);
}

void FileManager::add_folder(QString name){
    QFile* folder = new QFile(localfilesPath + name + ".txt");
    localFolders.insert(name, folder);
}

void FileManager::add_item_to_folder(const QString &item, const QString &folder)
{
    // Если мы перетягиваем в пустую область, то никуда добавлять не надо
    if(folder == ""){
        // В ДАЛЬНЕЙШЕМ ЗДЕСЬ, ВЕРОЯТНО БУДЕТ КАКОЕ-ТО ДОПОЛНИТЕЛЬНОЕ УДАЛЕНИЕ
        return;
    }

    QFile* target_folder = localFolders.value(folder);

    // Проверяем не содержится ли файл в этой папке, если да, то заканчиваем работу
    if(target_folder->open(QIODevice::ReadOnly)){
        QTextStream read(target_folder);
        QString line;
        while(!read.atEnd()){
            line = read.readLine();
            if(line == item){
                return;
            }
        }

        target_folder->close();
    }

    // Добавляем item в папку
    if(localFolders.contains(folder) && target_folder->open(QIODevice::Append)){
        QTextStream write(target_folder);
        write << item << Qt::endl;

        target_folder->close();
    }
}

void FileManager::add_item_to_folder(const QString &item, const QString &new_folder, const QString &old_folder)
{
    QFile* from_folder = localFolders.value(old_folder);

    // Перезаписываем старую папку
    if(from_folder->open(QIODevice::ReadOnly)){
        QTextStream read(from_folder);
        QStringList lines;
        QString line;
        while(!read.atEnd()){
            line = read.readLine();
            if(line != item){
                lines.append(line);
            }
        }

        from_folder->close();

        if(from_folder->open(QIODevice::WriteOnly)){
            QTextStream write(from_folder);
            foreach (auto line, lines) {
                write << line << Qt::endl;
            }

            from_folder->close();
        }
    }

    // Записываем файл в новую папку
    add_item_to_folder(item, new_folder);
}

void FileManager::remove_item_from_folder(const QString &item, const QString &folder)
{
    QFile* from_folder = localFolders.value(folder);

    // Если item в папке
    if(folder != ""){
        // Перезаписываем старую папку
        if(from_folder->open(QIODevice::ReadOnly)){
            QTextStream read(from_folder);
            QStringList lines;
            QString line;
            while(!read.atEnd()){
                line = read.readLine();
                if(line != item){
                    lines.append(line);
                }
            }

            from_folder->close();

            if(from_folder->open(QIODevice::WriteOnly)){
                QTextStream write(from_folder);
                foreach (auto line, lines) {
                    write << line << Qt::endl;
                }

                from_folder->close();
            }
        }
    }

    // Определяем является файлом или нет
    if(is_file(item)){
        // Удаляем файл из системы
        remove_file(item);
    }
    else{
        // Если это папка, то удаляем всех детей
        QFile* item_folder = localFolders.value(item);

        // Проверка на nullptr
        if(item_folder)
        {
            if(item_folder->open(QIODevice::ReadOnly)){
                QTextStream read(item_folder);
                QString line;
                while(!read.atEnd()){
                    line = read.readLine();
                    // Проверяем текущий элемент. Если файл, то просто удаляем его.
                    // Если это папка, то рекурсивно вызываем метод, чтобы удалить конкретно её в текущей папке
                    if(is_file(line))
                        remove_file(line);
                    else
                        remove_item_from_folder(line, item);
                }
                item_folder->close();
            }
            // Удаляем саму папку
            if(item_folder->remove()){
                localFolders.remove(item);
                delete item_folder;
            }
        }
        else{
            throw std::runtime_error("Folder wasn't found: " + item.toStdString());
        }
    }
}

void FileManager::rename_folder(const QString &old_name, const QString &new_name)
{
    QFile* renamed_folder = localFolders.value(old_name);

    QStringList lines;
    // Сохраняем все записи о папке
    if(renamed_folder->open(QIODevice::ReadOnly)){
        QTextStream read(renamed_folder);
        QString line;
        while(!read.atEnd()){
            line = read.readLine();
            lines.append(line);
        }
        renamed_folder->close();
    }

    // Удаляем папку и создаём новую
    if(renamed_folder->remove()){
        localFolders.remove(old_name);
        delete renamed_folder;
        renamed_folder = new QFile(localfilesPath + new_name + ".txt");
        localFolders.insert(new_name, renamed_folder);
    }

    // Добавляем все сохранённые строки в папку
    foreach(auto line, lines){
        add_item_to_folder(line, new_name);
    }
}

QStringList FileManager::get_existing_folders() const
{
    // Получение всех папок внури директории localfilesPath
    return localFolders.keys();
}


QStringList FileManager::get_existing_files() const
{
    // Получение всех файлов внури директории localfilesPath
    return localFiles.keys();
}

QStringList FileManager::get_children(const QString &folder_name) const
{
    QFile* folder = localFolders.value(folder_name);
    QStringList children;
    // Проходимся по всем строчкам и заполняем список
    if(folder->open(QIODevice::ReadOnly)){
        QTextStream read(folder);
        while (!read.atEnd()) {
            children.append(read.readLine());
        }

        folder->close();
    }
    return children;
}

void FileManager::remove_file(QString file_name)
{
    if(localFiles.contains(file_name)){
        QFile::remove(localFiles.value(file_name));
        localFiles.remove(file_name);
    }
}

bool FileManager::is_file(QString item_name){
    // Если в map есть такой элемент, то это файл
    return localFiles.contains(item_name);
}

QString FileManager::get_file_content(const QString &file_name)
{
    if(localFiles.contains(file_name)){
        QFile file(localFiles.value(file_name));
        if(file.open(QIODeviceBase::ReadOnly | QIODeviceBase::Text)){
            QTextStream in(&file);

            QString content = in.readAll();

            file.close();
            return content;
        }
    }
    return QString("Ошибка чтения файла");
}

QString FileManager::getFilePath(const QString& fileName) const
{
    // Если файл есть в map — возвращаем путь
    if (localFiles.contains(fileName)) {
        return localFiles.value(fileName);
    }
    // Если нет — пустая строка
    return "";
}

bool FileManager::saveDocument(const QString& fileName, const QString& content)
{
    if (!localFiles.contains(fileName)) {
        return false;
    }

    QString filePath = localFiles.value(fileName);

    // Если файл не HTML — извлекаем body
    if (!filePath.endsWith(".html", Qt::CaseInsensitive)) {
        QString bodyContent = extractBodyContent(content);
        write_to_file(fileName, bodyContent);
    } else {
        write_to_file(fileName, content);
    }

    return true;
}

QString FileManager::getFileExtension(const QString& fileName) const
{
    // 1. Сначала проверяем память (для новых файлов в сессии)
    if (localFileTypes.contains(fileName)) {
        return localFileTypes.value(fileName);
    }

    // 2. Если нет — пытаемся определить из имени файла
    QString filePath = localFiles.value(fileName);
    if (filePath.isEmpty()) return "html";

    QFileInfo fileInfo(filePath);
    QString baseName = fileInfo.completeBaseName(); // "Grafy.docx"

    // Разбиваем по точкам
    QStringList parts = baseName.split('.');
    if (parts.size() > 1) {
        QString ext = parts.last().toLower();
        if (ext == "docx" || ext == "doc" || ext == "md" || ext == "markdown") {
            return ext;
        }
    }

    return "html";
}


bool FileManager::loadDocument(const QString& fileName, QString& content)
{
    if (!localFiles.contains(fileName)) {
        return false;
    }

    QString filePath = localFiles.value(fileName);

    // Если это уже HTML — читаем как есть
    if (filePath.endsWith(".html", Qt::CaseInsensitive)) {
        content = get_file_content(fileName);
        return !content.isEmpty();
    }

    // Если это .docx/.md — конвертируем
    QString ext = QFileInfo(filePath).suffix().toLower();
    if (ext == "docx" || ext == "doc" || ext == "md" || ext == "markdown") {
        return ::loadDocument(filePath, content, getPandocPath());
    }

    return false;
}

QString FileManager::extractBodyContent(const QString& html)
{
    int bodyStart = html.indexOf("<body");
    if (bodyStart == -1) return html;

    bodyStart = html.indexOf(">", bodyStart) + 1;
    int bodyEnd = html.indexOf("</body>", bodyStart);

    if (bodyEnd == -1) return html;

    return html.mid(bodyStart, bodyEnd - bodyStart);
}


