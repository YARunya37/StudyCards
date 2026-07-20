#ifndef GROUPWINDOW_H
#define GROUPWINDOW_H

#include <QMainWindow>
#include "group.h"
#include <QListWidget>
#include "textformattingtoolbar.h"
class GroupWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit GroupWindow(Group* name, QWidget *parent = nullptr);

private slots:
    void add_new_card();

    void showContextMenu(const QPoint &pos);

    void deleteItem();
private:
    QWidget* content = nullptr;
    QListWidget* cardList = nullptr;

    // Группа с которой мы работаем
    Group* group;

    // Виджет билета
    StudyCardWidget* active_card = nullptr;

    void setupUI();

    void setCard(const QString& card_id);
    // Создаёт панель с кнопками. Возвращает созданную панель
    QFrame* create_button_panel();
};

#endif // GROUPWINDOW_H
