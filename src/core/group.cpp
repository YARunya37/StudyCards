#include "group.h"
#include <QCoreApplication>

Group::Group(const QString& name)
    : name{name},
    DirItemsManager(QCoreApplication::applicationDirPath() + "/resources/usergroups/" + name)
{
    foreach(auto item, RestoreItems()){
        auto card = new StudyCardWidget();
        cards.insert(item, card);
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
