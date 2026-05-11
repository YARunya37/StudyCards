#ifndef TEMPCARDSWIDGET_H
#define TEMPCARDSWIDGET_H

#include <QTabWidget>
#include <QWidget>
#include "group.h"

class TempCardsWidget : public QTabWidget
{
    Q_OBJECT
public:
    TempCardsWidget(QWidget* parent = nullptr);

public slots:
    void onActiveGroupChanged(Group* new_active_group);

private slots:
    // Создание пустой страницы
    void createEmptyPage();

private:
    // UI при первом запуске
    void setupInitUI();

    // Интерфейс временных билетов
    void setupTempCardUI();

    // Создаёт пустой билет
    StudyCardWidget* createEmptyCard(QWidget* parent);

    Group* curr_group;
};

#endif // TEMPCARDSWIDGET_H
