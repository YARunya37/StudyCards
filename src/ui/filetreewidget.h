#ifndef FILETREEWIDGET_H
#define FILETREEWIDGET_H

#include <QTreeWidget>
#include <QStringList>
#include <QMap>
#include <QString>
class FileTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    FileTreeWidget(QWidget* parent = nullptr);
public slots:
    void AddFiles();
private:
    // Массив файлов в проекте <имя файла(ключ), путь к нему(значение)>
    QMap<QString, QString> localFiles;
    void setUpTree();
    // Возвращает имя файла С РАСШИРЕНИЕМ(.docx .md ...)
    QString GetName(QString file);
};

#endif // FILETREEWIDGET_H
