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

void GroupManager::RenameGroup(const QString &old_name, const QString &new_name)
{
    if(RenameItem(old_name, new_name)){
        auto group = GetGroup(old_name);
        // Проверка на nullptr
        if(!group)
            return;

        // Удаление группы
        groups.remove(old_name);

        // Вставка с новым именем
        groups.insert(new_name, group);
        // Установка нового имени группе
        group->SetName(new_name);

        // Если это была активная группа, то запускаем сигнал об изменении
        if(active_group && old_name == active_group->Name()){
            emit active_group_changed(active_group);
        }
    }
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
        // Если удаляем активную группу, то зануляем её
        if(active_group && group_name == active_group->Name()){
            active_group = nullptr;
            emit active_group_changed(active_group);
        }

        auto group = GetGroup(group_name);
        // Проверка на nullptr
        if(!group)
            return;

        delete group;
        groups.remove(group_name);
    }
    else{
        qInfo() << "Ошибка при удалении группы";
    }
}

bool GroupManager::SetActiveGroup(const QString &group_name)
{
    auto group = GetGroup(group_name);

    if(group){
        active_group = group;

        emit active_group_changed(active_group);
        return true;
    }
    return false;
}
