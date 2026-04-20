#ifndef FILEMANAGER_H
#define FILEMANAGER_H


#include <QMap>
#include <QFile>

// Класс, отвечающий за все файлы, папки добавляемые в проект пользователем
class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject* parent);
    // Метод для добавления файлов внутрь проекта. Возвращает все добавленные файлы
    QStringList add_files(QStringList files);
    // Метод для сохранения паки внутри проекта
    void add_folder(QString name);

    void add_item_to_folder(const QString& item, const QString& folder);

    void remove_item_from_folder(const QString& item, const QString& folder);
    // Метод для получения всех файлов из директории проекта
    QStringList get_existing_files();
    // Метод для удаления файла с указанным именем
    void remove_file(QString file_name);
    // Метод, который возващает true если существует файл с указанным именем
    bool is_file(QString item_name);
public slots:
    // void OnFolderNameChanged();
private:
    // Массив файлов в проекте <имя файла(ключ), путь к нему(значение)>
    // Внутри проекта все файлы имеют путь /resources/userfiles/
    static QMap<QString, QString> localFiles;
    // Массив, хранящий файлы с информацией о папках в проекте <имя файла, сам файл>
    static QMap<QString,QFile*> localFolders;
    // Путь к директориии с файлами, добавленными пользователем
    QString localfilesPath;
    // Возвращает имя файла С РАСШИРЕНИЕМ(.docx .md ...) по его пути
    QString GetName(QString filePath);
};

#endif // FILEMANAGER_H
