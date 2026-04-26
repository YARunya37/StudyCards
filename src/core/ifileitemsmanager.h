#ifndef IFILEITEMSMANAGER_H
#define IFILEITEMSMANAGER_H

#include "fileitemsdirectory.h"


class IFileItemsManager
{
public:
    IFileItemsManager();

    // Метод для добавления файлового объекта по имени
    virtual void AddItem(const QString& name) = 0;
    // Метод для удаления файлового объекта по имени
    virtual void DeleteItem(const QString& name) = 0;
    // Метод для восстановления уже существующих объектов
    virtual void RestoreItems() = 0;

    virtual ~IFileItemsManager();
};

#endif // IFILEITEMSMANAGER_H
