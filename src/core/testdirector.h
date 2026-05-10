#ifndef TESTDIRECTOR_H
#define TESTDIRECTOR_H

#include <QObject>
#include <QQueue>
#include "testgenerator.h"
#include "base_question.h"

class TestDirector : public QObject
{
    Q_OBJECT
public:
    explicit TestDirector(Group* refGroup, QObject *parent = nullptr);
    ~TestDirector();

    // Методы управления тестом
    void StartTest();

    // Геттер текущего вопроса
    Question* GetCurrentQuestion() const;

    int currQuestionCount() const;
signals:
    void answerReceived(); // Сигнал о том, что ответ получен
    void testFinished();   // Сигнал о завершении теста

public slots:
    void NextQuestion();           // Переключение на следующий вопрос
    void onAnswerReceived(bool isCorrect); // Обработка ответа

private:
    void clearQueue();

private:
    Group* m_refGroup;
    TestGenerator* m_testGenerator;
    QQueue<Question*> m_questionQueue;
    Question* m_currentQuestion;
};

#endif // TESTDIRECTOR_H
