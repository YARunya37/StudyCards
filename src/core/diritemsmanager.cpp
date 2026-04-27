#include "diritemsmanager.h"
#include <QDebug>


DirItemsManager::DirItemsManager(const QString& path_to_working_dir)
    : currDir(path_to_working_dir)
{}

bool DirItemsManager::AddItem(const QString &name)
{
    // Если такой папки нет, то создаём с заданным именем
    if(!currDir.exists(name)){
        if(currDir.mkdir(name)){
            return true;
        }
    }

    return false;
}

bool DirItemsManager::DeleteItem(const QString &name)
{
    // Если существует папка с таким именем, то удаляем её
    if(currDir.exists(name)){
        if(currDir.remove(name)){
            return true;
        }
    }

    return false;
}

QStringList DirItemsManager::RestoreItems()
{
    // Возвращаем все папки, кроме специальных, системных
    return currDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}




