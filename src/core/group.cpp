#include "group.h"
#include <QCoreApplication>

Group::Group(const QString& name)
    : name{name},
    group_path{QCoreApplication::applicationDirPath() + "/resources/usergroups/" + name},
    DirItemsManager(group_path)
{
    foreach(auto item, RestoreItems()){
        auto card = new StudyCardWidget(nullptr, group_path + "/" + item, item);
        cards.insert(item, card);
    }
}

void Group::SetName(const QString &new_name)
{
    if(new_name != ""){
        this->name = new_name;
    }
}

StudyCardWidget* Group::GetCard(const QString &name) const
{
    return cards.value(name);
}

void Group::Clear()
{
    foreach (auto item, cards.keys()) {
        DeleteItem(item);
    }
}
