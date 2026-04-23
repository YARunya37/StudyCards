#ifndef GROUPMANAGER_H
#define GROUPMANAGER_H

#include <QObject>

class GroupManager : public QObject
{
    Q_OBJECT
public:
    explicit GroupManager(QObject *parent = nullptr);

signals:
};

#endif // GROUPMANAGER_H
