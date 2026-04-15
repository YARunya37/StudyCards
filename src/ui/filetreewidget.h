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
private slots:
    // Слот для вызова кастомного контекстного меню
    void showContextMenu(const QPoint& pos);
    // Слот для создания папки в виджете
    void createFolder();
    // Слот для удаления любого(файла, папки) элемента в виджете
    void deleteItem();
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
    // Метод для удаления дочерних элементов(файлов и папок) папки
    void deleteChildren(QTreeWidgetItem* folder);
};

#endif // FILETREEWIDGET_H
