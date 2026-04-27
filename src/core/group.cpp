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
    if(cards.keys().contains(name))
        return cards.value(name);
    else
        return nullptr;
}

void Group::Clear()
{
    foreach (auto item, cards.keys()) {
        DeleteItem(item);
    }
}

void Group::CreateCard(const QString &card_name)
{
    if(AddItem(card_name)){
        cards.insert(card_name, new StudyCardWidget(nullptr, group_path + "/" + card_name, card_name));
    }
    else{
        qInfo() << "Ошибка при создании группы";
    }
}

void Group::DeleteCard(const QString &card_name)
{
    if(DeleteItem(card_name)){
        auto card = GetCard(card_name);
        delete card;
        cards.remove(card_name);
    }
    else{
        qInfo() << "Ошибка при удалении группы";
    }
}

void Group::RenameCard(const QString &old_name, const QString &new_name)
{
    if(RenameItem(old_name, new_name)){
        auto card = GetCard(old_name);
        // Удаление группы
        cards.remove(old_name);

        // Вставка с новым именем
        cards.insert(new_name, card);
        // Установка нового имени группе
        card->SetName(new_name);
    }
}
