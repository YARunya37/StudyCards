#ifndef GROUP_H
#define GROUP_H

#include <QString>
// Класс, отвечающий за конкретную группу пользователя
class Group
{
public:
    Group(const QString& name);

    // Метод для получение имени группы
    QString Name() const;
private:
    // Поле с именем группы
    QString name;
};

#endif // GROUP_H
