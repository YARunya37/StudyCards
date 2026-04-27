#include "groupsuicontroller.h"
#include "groupcreationdialog.h"

GroupsUIController::GroupsUIController(QObject *parent)
    : QObject{parent},
    gm{new GroupManager(parent)}
{}

void GroupsUIController::show_creation_group_dialog()
{
    // Насильно приводим QObject предаваемый через parent, т.к. это MainWindow(QWidget)
    GroupCreationDialog dialog = GroupCreationDialog(qobject_cast<QWidget*>(this->parent()));

    // Запускаем диалог. Если пользователь принимает имя, то отправляем его на создание
    if (dialog.exec() == QDialog::Accepted)
    {
        QString group_name = dialog.getText();
        gm->CreateGroup(group_name);
    }
}
