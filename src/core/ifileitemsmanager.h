#ifndef IFILEITEMSMANAGER_H
#define IFILEITEMSMANAGER_H

#include <QString>

class IFileItemsManager
{
public:
    IFileItemsManager();

    // Метод для добавления файлового объекта по имени. Возвращает true, если успешно добавил элемент
    virtual bool AddItem(const QString& name) = 0;
    // Метод для удаления файлового объекта по имени. Возвращает true, если успешно удалил элемент
    virtual bool DeleteItem(const QString& name) = 0;
    // Метод для восстановления уже существующих объектов
    virtual QStringList RestoreItems() = 0;

    virtual ~IFileItemsManager();
};

#endif // IFILEITEMSMANAGER_H
