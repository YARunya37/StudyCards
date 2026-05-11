#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QObject>
#include <QString>

class FileManager;
class FileLoader;

class DocumentManager : public QObject
{
    Q_OBJECT

public:
    explicit DocumentManager(FileManager* fileManager, QObject* parent = nullptr);

    // Текущий документ
    QString currentFileName() const { return m_currentFileName; }
    bool isModified() const { return m_isModified; }

    // Операции
    bool loadDocument(const QString& fileName);
    bool saveDocument();
    bool saveDocumentAs(const QString& newFileName);
    void setContent(const QString& content);
    QString content() const { return m_content; }

    // Сброс флага изменений
    void setModified(bool modified);

signals:
    void contentLoaded(const QString& content);
    void documentSaved();
    void modificationChanged(bool modified);
    void currentDocumentChanged(const QString& fileName);

private:
    FileManager* m_fileManager;
    QString m_currentFileName;
    QString m_content;
    bool m_isModified;
    bool m_isHtmlFile;
    void addCssToHtml(QString& html);
};

#endif // DOCUMENTMANAGER_H
