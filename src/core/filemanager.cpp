#include "filemanager.h"
#include <QCoreApplication>
#include <QDir>

QMap<QString, QString> FileManager::localFiles;

FileManager::FileManager()
{
    localfilesPath = QCoreApplication::applicationDirPath() + "/resources/userfiles/";
    // Восстанавливаем на основе файлов map
    foreach (auto file, QDir(localfilesPath).entryList(QDir::Files)) {
        // Если файл, то добавляем в map файлов
        localFiles.insert(file.split(".")[0], localfilesPath + file);
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
