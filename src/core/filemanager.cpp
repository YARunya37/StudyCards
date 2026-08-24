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
        if (file.endsWith(".txt", Qt::CaseInsensitive)) {
            // Файл с информацией о папке: отрезаем только ".txt"
            localFolders.insert(QFileInfo(file).completeBaseName(), new QFile(localfilesPath + file));
        }
        else{
            // Файл: отрезаем только последнее расширение (.html), точки в имени сохраняем
            localFiles.insert(QFileInfo(file).completeBaseName(), localfilesPath + file);
        }
    }
}

QStringList FileManager::add_files(QStringList files)
{
    QStringList added_files = QStringList();
    QStringList rejected_files = QStringList();  // Добавляем список отклонённых

    // Все указанные файлы конвертируем и добавляем в папку с файлами проекта
    foreach (auto filePath, files) {

        // Обрезаем только последнее расширение, точки в имени сохраняем
        QString name = QFileInfo(filePath).completeBaseName();
        if (name.isEmpty()) {
            name = QFileInfo(filePath).fileName();
        }
        QString ext = QFileInfo(filePath).suffix().toLower();

        // Проверка: не занято ли имя
        if(isNameTaken(name)){
            qWarning() << "FileManager: Cannot add file - name already taken:" << name;
            rejected_files.append(name);  // Добавляем в список отклонённых
            continue;  // Пропускаем этот файл
        }
        // Если файла с таким именем ещё нет, то добавляем его в дерево
        if(!localFiles.contains(name)){
            QString destName = name + ".html";  // Сохраняем как HTML
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
                        added_files.append(name);
                    }
                }
            } else if (ext == "html") {
                // Просто копируем HTML
                if (QFile::copy(filePath, dest)) {
                    localFiles.insert(name, dest);
                    added_files.append(name);
                }
            }
        }
    }

    // Сообщаем о файлах, которые не были добавлены
    if (!rejected_files.isEmpty()) {
        emit filesAddRejected(rejected_files);
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
    // Проверка: не занято ли имя
    if(isNameTaken(name)){
        qWarning() << "FileManager: Cannot create folder - name already taken:" << name;
        return;
    }
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

    if (!target_folder) {
        qWarning() << "FileManager: Folder not found or is null:" << folder;
        return;
    }

    // Проверяем не содержится ли файл в этой папке, если да, то заканчиваем работу
    if(target_folder->open(QIODevice::ReadOnly)){
        QTextStream read(target_folder);
        QString line;
        while(!read.atEnd()){
            line = read.readLine();
            if(line == item){
                target_folder->close();
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
        if (!from_folder) {
            qWarning() << "FileManager: Folder not found:" << folder;
            return;
        }
        // Перезаписываем старую папку
        if(from_folder->open(QIODevice::ReadOnly)){
            QTextStream read(from_folder);

            QStringList lines;
            QString line;
            while(!read.atEnd()){
                line = read.readLine().trimmed();
                if(line != item && !line.isEmpty()){
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
        else {
            qWarning() << "FileManager: Folder not found (may be already deleted):" << item;
            // Просто удаляем из UI
            return;  // Выходим без ошибки
        }
    }

    // Определяем является файлом или нет
    if(is_file(item)){
        // Удаляем файл из системы
        remove_file(item);
    }
    else if(is_folder(item)){
        // Если это папка, то удаляем всех детей
        QFile* item_folder = localFolders.value(item);

        // Проверка на nullptr
        if(item_folder)
        {
            if(item_folder->open(QIODevice::ReadOnly)){
                QTextStream read(item_folder);
                QStringList children;
                QString line;

                while(!read.atEnd()){
                    line = read.readLine().trimmed();
                    // Проверяем текущий элемент. Если файл, то просто удаляем его.
                    // Если это папка, то рекурсивно вызываем метод, чтобы удалить конкретно её в текущей папке
                    if(!line.isEmpty()){
                        children.append(line);
                    }
                }
                item_folder->close();

                foreach(const QString& child, children){
                    if(is_file(child))
                        remove_file(child);
                    else
                        remove_item_from_folder(child, item);
                }
            }

            // Удаляем саму папку
            if(item_folder->remove()){
                localFolders.remove(item);
                delete item_folder;
            }
        }
        else{
            qWarning() << "FileManager: Folder exists in map but pointer is null:" << item;
            // Просто выходим, не крашим приложение
            return;
        }
    }
    else {
        qWarning() << "FileManager: Item not found:" << item;
    }
}

void FileManager::rename_folder(const QString& old_name, const QString& new_name)
{
    QFile* renamed_folder = localFolders.value(old_name);
    if (!renamed_folder) {
        qWarning() << "FileManager: Cannot rename - folder not found:" << old_name;
        return;
    }

    // Переименовываем файл на диске
    QString old_path = localfilesPath + old_name + ".txt";
    QString new_path = localfilesPath + new_name + ".txt";

    if (!QFile::rename(old_path, new_path)) {
        qWarning() << "FileManager: Failed to rename file from" << old_path << "to" << new_path;
        return;
    }

    // Обновляем map
    localFolders.remove(old_name);
    delete renamed_folder;

    QFile* new_folder = new QFile(new_path);
    localFolders.insert(new_name, new_folder);
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

bool FileManager::is_folder(QString item_name){
    return localFolders.contains(item_name);
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

QString FileManager::GetName(QString file)
{
    QString name = "";
    // Циклом справа налево записываем символы в строку
    for (int i = file.length()-1; i >= 0; i--) {
        if(file[i] != "/" && file[i] != "\\"){
            name = file[i] + name;
        }
        else{
            // Если мы дошли до / или \, то имя записано полностью
            break;
        }
    }

    return name;
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

bool FileManager::isNameTaken(const QString& name) const
{
    return localFiles.contains(name) || localFolders.contains(name);
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
