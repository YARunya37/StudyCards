#ifndef FILETREEWIDGET_H
#define FILETREEWIDGET_H

#include <QTreeWidget>
#include <QStringList>
#include <QString>
#include "filemanager.h"
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
    // Переименовать папку, возвращает старое имя
    QString renameItem();
protected:
    // Переопределение метода перетягивания элементов
    void dropEvent(QDropEvent* event) override;
private:
    // Класс для управления файлами в проекте
    FileManager* fmn;
    // Создаёт виджет(форма, стиль и необходимый функционал)
    void setUpTree();
    // Отображает уже сохранённые пользователем файлы
    void restoreState();
    // Метод для удаления дочерних элементов(файлов и папок) папки
    void deleteChildren(QTreeWidgetItem* folder);
};

#endif // FILETREEWIDGET_H
