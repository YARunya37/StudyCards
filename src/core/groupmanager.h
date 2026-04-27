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

signals:

private:
    // Список всех групп по именам
    QMap<QString, Group*> groups;
};

#endif // GROUPMANAGER_H
