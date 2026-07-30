#include "diritemsmanager.h"
#include <QDebug>

namespace
{
    const QString ForbiddenCharacters = ".*\\/<>?:\"|";

    const QString InvalidNameError =
        "Имя не должно содержать символы:\n"
        ". * / \\ : < > ? \" |";

    const QString ItemAlreadyExistsError =
        "Объект с таким именем уже существует.";

    const QString ItemNotFoundError =
        "Объект с таким именем не существует.";

    const QString CreateItemError =
        "Не удалось создать объект из-за ошибки файловой системы.";

    const QString DeleteItemError =
        "Не удалось удалить объект из-за ошибки файловой системы.";

    const QString RenameItemError =
        "Не удалось переименовать объект из-за ошибки файловой системы.";
}

DirItemsManager::DirItemsManager(const QString& path_to_working_dir)
    : currDir{QDir(path_to_working_dir)}
{}

QString DirItemsManager::GetLastError() const
{
    return lastError;
}

bool DirItemsManager::AddItem(const QString &name)
{
    lastError.clear();

    if (!IsValidItemName(name))
    {
        lastError = InvalidNameError;
        return false;
    }

    if (currDir.exists(name))
    {
        lastError = ItemAlreadyExistsError;
        return false;
    }

    if (currDir.mkdir(name))
    {
        return true;
    }

    lastError = CreateItemError;
    return false;
}

bool DirItemsManager::DeleteItem(const QString &name)
{
    lastError.clear();

    if (!currDir.exists(name))
    {
        lastError = ItemNotFoundError;
        return false;
    }

    QDir dirToRemove(currDir.filePath(name));

    if (dirToRemove.removeRecursively())
    {
        return true;
    }

    lastError = DeleteItemError;
    return false;
}

bool DirItemsManager::RenameItem(const QString &old_name, const QString &new_name)
{
    lastError.clear();

    if (!IsValidItemName(new_name))
    {
        lastError = InvalidNameError;
        return false;
    }

    if (!currDir.exists(old_name))
    {
        lastError = ItemNotFoundError;
        return false;
    }

    if (currDir.exists(new_name))
    {
        lastError = ItemAlreadyExistsError;
        return false;
    }

    if (currDir.rename(old_name, new_name))
    {
        return true;
    }

    lastError = RenameItemError;
    return false;
}

QStringList DirItemsManager::RestoreItems()
{
    // Возвращаем все папки, кроме специальных, системных
    return currDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
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




