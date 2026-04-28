#ifndef GROUPWINDOW_H
#define GROUPWINDOW_H

#include <QMainWindow>
#include "group.h"

class GroupWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit GroupWindow(Group* name, QWidget *parent = nullptr);


private:
    // Группа с которой мы работаем
    Group* group;

    // Виджет билета
    StudyCardWidget* active_card = nullptr;
    void setupUI();

    // Создаёт панель с кнопками. Возвращает созданную панель
    QFrame* create_button_panel();
};

#endif // GROUPWINDOW_H
