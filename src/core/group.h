#ifndef GROUP_H
#define GROUP_H

#include <QString>
#include "studycard.h"
#include <QMap>
#include "diritemsmanager.h"

// Класс, отвечающий за конкретную группу пользователя
class Group : public DirItemsManager
{
public:
    Group(const QString& name);
    // Метод для получение имени группы
    QString Name() const;
    // Метод для получения указателя на билет по имени
    StudyCardWidget* GetCard(const QString& name) const;
    // Метод для удаления всех билетов в группе
    void Clear();
private:
    // Поле с именем группы
    QString name;
    // Список билетов
    QMap<QString, StudyCardWidget*> cards;
    // Путь к группе
    const QString group_path;

};

#endif // GROUP_H
