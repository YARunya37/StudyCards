#ifndef GROUP_H
#define GROUP_H

#include <QString>
#include "studycard.h"
#include <QMap>
#include "diritemsmanager.h"
#include "namedfileitem.h"
// Класс, отвечающий за конкретную группу пользователя
class Group : public DirItemsManager, public NamedFileItem
{
public:
    Group(const QString& name);
    // Метод для получения указателя на билет по имени
    StudyCardWidget* GetCard(const QString& name) const;
    // Метод для удаления всех билетов в группе
    void Clear();
public slots:
    void CreateCard(const QString& card_name);
    void DeleteCard(const QString& card_name);
    void RenameCard(const QString& old_name, const QString& new_name);
private:
    // Список билетов
    QMap<QString, StudyCardWidget*> cards;
    // Путь к группе
    const QString group_path;

};

#endif // GROUP_H
