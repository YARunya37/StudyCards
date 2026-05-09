#ifndef SIMPLEQUESTION_H
#define SIMPLEQUESTION_H

#include "base_question.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>

class SimpleQuestion : public Question
{
    Q_OBJECT
public:
    SimpleQuestion(StudyCardWidget *referenceCard, QWidget *parent = nullptr);

private:
    void setup_face();
    void setup_back();

private slots:
    void onShowAnswerClicked();
    void onCorrectClicked();
    void onIncorrectClicked();

private:
    // Элементы лицевой стороны
    QTextEdit *questionText = nullptr;
    QPushButton *showAnswerButton = nullptr;

    // Элементы обратной стороны
    QTextEdit *answerText = nullptr;
    QPushButton *correctButton = nullptr;
    QPushButton *incorrectButton = nullptr;
};


#endif // SIMPLEQUESTION_H
