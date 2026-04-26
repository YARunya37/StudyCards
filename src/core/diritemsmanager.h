#ifndef DIRITEMSMANAGER_H
#define DIRITEMSMANAGER_H

#include "ifileitemsmanager.h"

class DirItemsManager : public IFileItemsManager
{
public:
    DirItemsManager(const QString& path_to_working_dir);

protected:
    // Текущая рабочая директория
    QDir currDir;

    // Метод для добавления файлового объекта по имени
    void AddItem(const QString& name) override;
    // Метод для удаления файлового объекта по имени
    void DeleteItem(const QString& name) override;
    // Метод для восстановления уже существующих объектов
    void RestoreItems() override;
};

#endif // DIRITEMSMANAGER_H
