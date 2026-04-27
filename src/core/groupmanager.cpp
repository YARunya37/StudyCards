#include "groupmanager.h"
#include <QCoreApplication>
GroupManager::GroupManager(QObject *parent)
    : QObject{parent},
    DirItemsManager(QCoreApplication::applicationDirPath() + "/resources/usergroups")
{
    // Инициализируем список
    foreach (auto item, RestoreItems()) {
        groups.insert(item, new Group(item));
    }
}

Group *GroupManager::GetGroup(const QString &group_name)
{
    if(groups.contains(group_name)){
        return groups.value(group_name);
    }
    return nullptr;
}

void GroupManager::CreateGroup(const QString &group_name)
{
    if(AddItem(group_name)){
        groups.insert(group_name, new Group(group_name));
    }
    else{
        qInfo() << "Ошибка при создании группы";
    }
}

void GroupManager::DeleteGroup(const QString &group_name)
{
    if(DeleteItem(group_name)){
        groups.remove(group_name);
    }
    else{
        qInfo() << "Ошибка при удалении группы";
    }
}
