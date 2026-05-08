#include "groupsuicontroller.h"
#include "textinputdialog.h"
#include "groupwindow.h"
#include <QInputDialog>
#include <QMessageBox>
GroupsUIController::GroupsUIController(QObject *parent)
    : QObject{parent},
    gm{new GroupManager(parent)}
{
    // Для тестов выбора активной группы
    connect(gm, &GroupManager::active_group_changed, this, [this](Group* new_active_group){
        qInfo() << "Активная группа" << new_active_group->Name();
    });
}

void GroupsUIController::show_creation_group_dialog()
{
    // Насильно приводим QObject предаваемый через parent, т.к. это MainWindow(QWidget)
    TextInputDialog dialog = TextInputDialog(qobject_cast<QWidget*>(this->parent()), "Введите название группы", "Название:");

    // Запускаем диалог. Если пользователь принимает имя, то отправляем его на создание
    if (dialog.exec() == QDialog::Accepted)
    {
        QString group_name = dialog.getText();
        // Если не удалось создать группу выводим сообщение об ошибку
        if(!gm->CreateGroup(group_name)){
            QMessageBox::warning(
                qobject_cast<QWidget*>(this->parent()),                          // parent
                "Ошибка создания группы",                // заголовок
                "Группа с именем \"" + group_name + "\" уже существует.\n"
                 "Пожалуйста, выберите другое имя.",     // текст
                QMessageBox::Ok                          // кнопки
                );
        }else {
            // Уведомление об успешном создании группы
            QMessageBox::information(
                qobject_cast<QWidget*>(this->parent()),                          // parent
                "Успешно",                               // заголовок
                "Группа \"" + group_name + "\" успешно создана.",  // текст
                QMessageBox::Ok                          // кнопки
                );
        }

    }
}

void GroupsUIController::show_delete_group_window()
{
    QStringList groups = gm->GetAvaliableGroups();

    if (groups.isEmpty())
    {
        QMessageBox::information(qobject_cast<QWidget*>(this->parent()), "Нет групп",
                                 "Нет доступных групп для удаления.");
        return;
    }

    bool ok;
    QString selectedGroup = QInputDialog::getItem(
        qobject_cast<QWidget*>(this->parent()),
        "Удаление группы",
        "Выберите группу для удаления:",
        groups,
        0,          // Текущий индекс
        false,      // Редактируемый (false = нельзя вводить вручную)
        &ok
        );

    if (ok && !selectedGroup.isEmpty())
    {
        // Подтверждение
        QMessageBox::StandardButton reply = QMessageBox::question(
            qobject_cast<QWidget*>(this->parent()),
            "Подтверждение",
            QString("Удалить группу \"%1\"?").arg(selectedGroup),
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::Yes)
        {
            gm->DeleteGroup(selectedGroup);
        }
    }
}

void GroupsUIController::choose_active_group()
{
    QStringList groups = gm->GetAvaliableGroups();

    if (groups.isEmpty())
    {
        QMessageBox::information(qobject_cast<QWidget*>(this->parent()), "Нет групп",
                                 "Нет доступных групп для выбора.");
        return;
    }

    bool ok;
    QString selectedGroup = QInputDialog::getItem(
        qobject_cast<QWidget*>(this->parent()),
        "Выбор активной группы",
        "Выберите группу для удаления:",
        groups,
        0,          // Текущий индекс
        false,      // Редактируемый (false = нельзя вводить вручную)
        &ok
    );

    if(ok && !selectedGroup.isEmpty()){
        gm->SetActiveGroup(selectedGroup);
    }
}

void GroupsUIController::choose_group_to_open()
{
    QStringList groups = gm->GetAvaliableGroups();

    if (groups.isEmpty())
    {
        QMessageBox::information(qobject_cast<QWidget*>(this->parent()), "Нет групп",
                                 "Нет доступных групп для выбора.");
        return;
    }

    bool ok;
    QString selectedGroup = QInputDialog::getItem(
        qobject_cast<QWidget*>(this->parent()),
        "Выбор активной группы",
        "Выберите группу для удаления:",
        groups,
        0,          // Текущий индекс
        false,      // Редактируемый (false = нельзя вводить вручную)
        &ok
        );

    if(ok && !selectedGroup.isEmpty()){
        auto group = gm->GetGroup(selectedGroup);
        if(group){
            GroupWindow* window = new GroupWindow(gm->GetGroup(selectedGroup), qobject_cast<QWidget*>(this->parent()));
            window->show();
        }
    }
}
