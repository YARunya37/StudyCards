#include "filemanager.h"
#include <QCoreApplication>
#include <QDir>

QMap<QString, QString> FileManager::localFiles;
QMap<QString, QFile*> FileManager::localFolders;

FileManager::FileManager(QObject* parent)
    : QObject(parent)
{
    localfilesPath = QCoreApplication::applicationDirPath() + "/resources/userfiles/";
    // Восстанавливаем на основе файлов map
    foreach (auto file, QDir(localfilesPath).entryList(QDir::Files)) {
        if(file.split(".")[1] != "txt"){
            // Если файл, то добавляем в map файлов
            localFiles.insert(file.split(".")[0], localfilesPath + file);
        }
        else{
            // Добавляем в map папок
            localFolders.insert(file.split(".")[0], new QFile(localfilesPath + file));
            // Отправляем в filetreewidget имя папки и всех его членов
        }
    }
}

QStringList FileManager::add_files(QStringList files)
{
    QStringList added_files = QStringList();
    // Все указанные файлы конвертируем и добавляем в папку с файлами проекта
    foreach (auto filePath, files) {
        QString name = GetName(filePath).split(".")[0];

        // Если файла с таким именем ещё нет, то добавляем его в дерево
        if(!localFiles.contains(name)){
            // Путь к директории с файлами(внутри проекта) + имя данного файла с расширением
            QString dest = localfilesPath + GetName(filePath);

            // ЗДЕСЬ ДОЛЖНА БЫТЬ КОНВЕРТАЦИЯ ФАЙЛОВ И ЗАПИСЬ ИХ В НУЖНУЮ ПАПКУ
            if(QFile::copy(filePath, dest)){
                // Добавляем в map локальный файл
                localFiles.insert(name, dest);
                added_files.append(name);
            }
            // СДЕЛАТЬ УВЕДОМЛЕНИЕ ЧТО ФАЙЛЫ УЖЕ ДОБАЛЕНЫ
        }
    }
    return added_files;
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
