#ifndef GROUPMANAGER_H
#define GROUPMANAGER_H

#include <QObject>
#include "diritemsmanager.h"
#include <QMap>
#include "group.h"

class GroupManager : public QObject, public DirItemsManager
{
    Q_OBJECT
public:
    explicit GroupManager(QObject *parent = nullptr);
    // Метод для получения указателя на группу
    Group* GetGroup(const QString& group_name);

    // Метод для получения всех имён групп
    QStringList GetAvaliableGroups() const;
signals:
    void active_group_changed(Group* new_active_group);
public slots:
    bool RenameGroup(const QString& old_name, const QString& new_name);
    bool CreateGroup(const QString& group_name);
    bool DeleteGroup(const QString& group_name);
    // Метод для установки активной группы. Возвращает true, если всё успешно
    bool SetActiveGroup(const QString& group_name);
private:
    // Список всех групп по именам
    static QMap<QString, Group*> groups;
    // Активная группа
    static Group* active_group;
};

#endif // GROUPMANAGER_H
