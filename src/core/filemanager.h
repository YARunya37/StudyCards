#ifndef FILEMANAGER_H
#define FILEMANAGER_H


#include <QMap>
#include <QFile>

// Класс, отвечающий за все файлы, папки добавляемые в проект пользователем
class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject* parent, QString path);
    // Метод для ДОБАВЛЕНИЯ файлов внутрь проекта. Возвращает все добавленные файлы
    QStringList add_files(QStringList files);
    // Метод для СОЗДАНИЯ файлов внутри директории
    void create_files(QStringList file_names);
    // Метод для записи контента в файл
    void write_to_file(const QString& file_name, const QString& content);
    // Метод для сохранения паки внутри проекта
    void add_folder(QString name);
    // Метод для добавления записи о файле в папку
    void add_item_to_folder(const QString& item, const QString& folder);
    // Метод для перезаписи файла из одной папки в другую
    void add_item_to_folder(const QString& item, const QString& new_folder, const QString& old_folder);
    // Метод для полного удаления item из папки(проекта)
    void remove_item_from_folder(const QString& item, const QString& folder);
    //Метод для переименования папки внутри проекта
    void rename_folder(const QString& old_name, const QString& new_name);
    // Возвращает все папки, доступные внутри директории файлов
    QStringList get_existing_folders() const;
    // Метод для получения всех файлов из директории проекта
    QStringList get_existing_files() const;
    // Возвращает дочерние элементы к папке
    QStringList get_children(const QString& folder_name) const;
    // Метод для удаления файла с указанным именем
    void remove_file(QString file_name);
    // Метод, который возващает true если существует файл с указанным именем
    bool is_file(QString item_name);
    // Метод для получения пути файла
    QString getFilePath(const QString& fileName) const;
    // Метод для получения контента файла
    QString get_file_content(const QString& file_name);
public slots:
    // void OnFolderNameChanged();
private:
    // Массив файлов в проекте <имя файла(ключ), путь к нему(значение)>
    // Внутри проекта все файлы имеют путь /resources/userfiles/
    QMap<QString, QString> localFiles;
    // Массив, хранящий файлы с информацией о папках в проекте <имя файла, сам файл>
    QMap<QString,QFile*> localFolders;
    // Путь к директориии с файлами, добавленными пользователем
    QString localfilesPath;
    // Возвращает имя файла С РАСШИРЕНИЕМ(.docx .md ...) по его пути
    QString GetName(QString filePath);
};

#endif // FILEMANAGER_H
