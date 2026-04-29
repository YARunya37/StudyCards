#include "group.h"
#include <QCoreApplication>

Group::Group(const QString& name)
    : NamedFileItem(name),
    group_path{QCoreApplication::applicationDirPath() + "/resources/usergroups/" + name},
    DirItemsManager(QCoreApplication::applicationDirPath() + "/resources/usergroups/" + name)
{
    foreach(auto item, RestoreItems()){
        auto card = new StudyCardWidget(nullptr, group_path + "/" + item, item);
        cards.insert(item, card);
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

QStringList Group::GetAllCards() const
{
    return cards.keys();
}

bool Group::CreateCard(const QString &card_name)
{
    if(AddItem(card_name)){
        cards.insert(card_name, new StudyCardWidget(nullptr, group_path + "/" + card_name, card_name));
        return true;
    }
    else{
        qInfo() << "Ошибка при создании группы";
        return false;
    }
}

void Group::DeleteCard(const QString &card_name)
{
    if(DeleteItem(card_name)){
        auto card = GetCard(card_name);

        // Проверка на nullptr
        if(!card)
            return;

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

        // Проверка на nullptr
        if(!card)
            return;

        // Удаление группы
        cards.remove(old_name);

        // Вставка с новым именем
        cards.insert(new_name, card);
        // Установка нового имени группе
        card->SetName(new_name);
    }
}
