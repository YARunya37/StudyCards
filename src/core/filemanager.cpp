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


QStringList FileManager::get_existing_files()
{
    // Получение всех файлов внури директории localfilesPath
    return localFiles.keys();
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
