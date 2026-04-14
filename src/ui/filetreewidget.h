#ifndef FILETREEWIDGET_H
#define FILETREEWIDGET_H

#include <QTreeWidget>
#include <QStringList>
#include <QMap>
#include <QString>
// Виджет для отображения и управления файлами внутри проекта
class FileTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    FileTreeWidget(QWidget* parent = nullptr);
public slots:
    // Вызывает диалоговое окно для добавления выбранных файлов в виджет файловой системы
    void AddFiles();
protected:
    // Переопределение метода перетягивания элементов
    void dropEvent(QDropEvent* event) override;
private:
    // Массив файлов в проекте <имя файла(ключ), путь к нему(значение)>
    // Внутри проекта все файлы имеют путь /resources/userfiles/
    QMap<QString, QString> localFiles;
    // Создаёт виджет(форма, стиль и необходимый функционал)
    void setUpTree();
    // Возвращает имя файла С РАСШИРЕНИЕМ(.docx .md ...) по его пути
    QString GetName(QString filePath);
    // Отображает уже сохранённые пользователем файлы
    void restoreState();
};

#endif // FILETREEWIDGET_H
