#include "diritemsmanager.h"
#include <QDebug>


DirItemsManager::DirItemsManager(const QString& path_to_working_dir)
    : currDir{QDir(path_to_working_dir)}
{}

bool DirItemsManager::AddItem(const QString &name)
{
    if (!IsValidItemName(name))
    {
        return false;
    }

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
        QDir dirToRemove(currDir.filePath(name));
        if(dirToRemove.removeRecursively()){
            return true;
        }
    }

    return false;
}

bool DirItemsManager::RenameItem(const QString &old_name, const QString &new_name)
{
    if (!IsValidItemName(new_name))
    {
        return false;
    }

    if(currDir.rename(old_name, new_name)){
        return true;
    }

    return false;
}

QStringList DirItemsManager::RestoreItems()
{
    // Возвращаем все папки, кроме специальных, системных
    return currDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

// Набор запрещенных символов
namespace
{
    const QString ForbiddenCharacters = ".*\\/<>?:\"|";
}

bool DirItemsManager::IsValidItemName(const QString& name) const
{
    for (const QChar& character : name)
    {
        if (ForbiddenCharacters.contains(character))
        {
            qWarning() << "Недопустимое имя. Имя не должно содержать символы:"
                       << ". * / \\ : < > ? \" |";

            return false;
        }
    }

    return true;
}




