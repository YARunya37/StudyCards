#ifndef GROUPSUICONTROLLER_H
#define GROUPSUICONTROLLER_H

#include <QObject>
#include "groupmanager.h"

class GroupsUIController : public QObject
{
    Q_OBJECT
public:
    explicit GroupsUIController(QObject *parent = nullptr);
signals:
    void active_group_changed(Group* new_active_group);
public slots:
    void show_creation_group_dialog();

    void show_delete_group_window();

    void choose_active_group();

    void choose_group_to_open();
private:
    GroupManager* gm;
};

#endif // GROUPSUICONTROLLER_H
