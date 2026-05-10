#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "testdirector.h"
#include "base_question.h"

class TestWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TestWindow(Group* refGroup, QWidget *parent = nullptr);
    ~TestWindow();

private slots:
    void onStartButtonClicked();
    void onNextButtonClicked();
    void onAnswerReceived();
    void onTestFinished();

private:
    void setupUI();
    void updateHeaderInfo();
    void showStartScreen();
    void showQuestion(Question* question);
    void showFinishScreen();

private:
    Group* refGroup = nullptr;
    TestDirector* testDirector = nullptr;

    // UI элементы
    QVBoxLayout* mainLayout = nullptr;

    // Заголовок
    QLabel* groupNameLabel = nullptr;
    QLabel* questionCountLabel = nullptr;
    QWidget* headerWidget = nullptr;

    // Контейнер для контента (куда будем вставлять виджеты)
    QVBoxLayout* contentLayout = nullptr;
    QWidget* contentContainer = nullptr;
    QWidget* currentQuestionWidget = nullptr;  // Текущий показываемый виджет
    QLabel* message = nullptr;

    // Кнопка
    QPushButton* m_actionButton = nullptr;

    // Статистика
    bool m_testStarted = false;
};

#endif // TESTWINDOW_H
