#ifndef GROUPSUICONTROLLER_H
#define GROUPSUICONTROLLER_H

#include <QObject>
#include "groupmanager.h"

class GroupsUIController : public QObject
{
    Q_OBJECT
public:
    explicit GroupsUIController(QObject *parent = nullptr);

public slots:
    void show_creation_group_dialog();

private:
    GroupManager* gm;
};

#endif // GROUPSUICONTROLLER_H
