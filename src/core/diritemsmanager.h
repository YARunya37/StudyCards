#ifndef DIRITEMSMANAGER_H
#define DIRITEMSMANAGER_H

#include "ifileitemsmanager.h"
#include <QDir>

class DirItemsManager : public IFileItemsManager
{
public:
    DirItemsManager(const QString& path_to_working_dir);

protected:
    // Текущая рабочая директория
    QDir currDir;

    // Метод для добавления файлового объекта по имени
    bool AddItem(const QString& name) override;
    // Метод для удаления файлового объекта по имени
    bool DeleteItem(const QString& name) override;
    // Метод для восстановления уже существующих объектов
    QStringList RestoreItems() override;
};

#endif // DIRITEMSMANAGER_H
