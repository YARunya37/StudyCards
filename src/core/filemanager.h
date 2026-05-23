#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "FileLoader.h"

#include <QMap>
#include <QFile>
#include <QString>

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
    // Метод, который возващает true если существует папка с указанным именем
    bool is_folder(QString item_name);
    // Метод для получения пути файла
    QString getFilePath(const QString& fileName) const;
    // Метод для получения контента файла
    QString get_file_content(const QString& file_name);
    // Устанавливает текущий файл, сбрасывает m_isModified
    void setCurrentFile(const QString& fileName) { m_currentFile = fileName; m_isModified = false; }
    // Устанавливает флаг изменения
    void setModified(bool modified) { m_isModified = modified; }
    // Проверяет были ли изменения
    bool isModified() const { return m_isModified; }
    // Возвращает имя текущего файла
    QString currentFile() const { return m_currentFile; }
    // Загружает документ: .html читает как есть, .docx/.md конвертирует через pandoc
    bool loadDocument(const QString& fileName, QString& content);
    // Извлекает содержимое <body> из HTML (статический метод)
    static QString extractBodyContent(const QString& html);
    // Сохраняет документ: если не .html — извлекает только <body>
    bool saveDocument(const QString& fileName, const QString& content);
    // Проверка, занято ли имя (файлом или папкой)
    bool isNameTaken(const QString& name) const;

signals:
    void fileContentChanged(const QString& fileName, const QString& content); // Контент файла изменился
    void fileSaved(const QString& fileName); // Файл сохранён
    void modificationChanged(bool modified); // Изменился флаг modified
    void currentFileChanged(const QString& fileName); // Сменился текущий файл

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
    // Имя текущего открытого файла
    QString m_currentFile;
    // Флаг: были ли изменения в текущем файле
    bool m_isModified;// Были ли изменения

};

#endif // FILEMANAGER_H
