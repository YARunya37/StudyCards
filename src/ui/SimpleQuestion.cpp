#include "SimpleQuestion.h"

SimpleQuestion::SimpleQuestion(StudyCardWidget *referenceCard, QWidget *parent)
    : Question(referenceCard, parent)
{
    setup_face();
    setup_back();
}

void SimpleQuestion::setup_face()
{
    // Создаем layout для лицевой стороны
    QVBoxLayout *faceLayout = new QVBoxLayout(face);

    // Текст вопроса
    questionText = new QTextEdit(face);
    questionText->setHtml(refCard->GetHeaderContent());
    questionText->setReadOnly(true);

    // Кнопка показать ответ
    showAnswerButton = new QPushButton("Показать ответ", face);
    connect(showAnswerButton, &QPushButton::clicked, this, &SimpleQuestion::onShowAnswerClicked);

    // Добавляем в layout
    faceLayout->addWidget(questionText);
    faceLayout->addWidget(showAnswerButton);
}

void SimpleQuestion::setup_back()
{
    // Создаем layout для обратной стороны
    QVBoxLayout *backLayout = new QVBoxLayout(back);

    // Текст ответа
    answerText = new QTextEdit(back);
    answerText->setHtml(refCard->GetBodyContent());
    answerText->setReadOnly(true);

    // Кнопки верно/неверно
    correctButton = new QPushButton("Верно", back);
    incorrectButton = new QPushButton("Неверно", back);

    connect(correctButton, &QPushButton::clicked, this, &SimpleQuestion::onCorrectClicked);
    connect(incorrectButton, &QPushButton::clicked, this, &SimpleQuestion::onIncorrectClicked);

    // Layout для кнопок
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(correctButton);
    buttonsLayout->addWidget(incorrectButton);

    // Добавляем в основной layout
    backLayout->addWidget(answerText);
    backLayout->addLayout(buttonsLayout);
}

Question *SimpleQuestion::Clone() const
{
    return new SimpleQuestion(refCard, nullptr);
}

void SimpleQuestion::onShowAnswerClicked()
{
    ShowBack();
}

void SimpleQuestion::onCorrectClicked()
{
    answer = true;
    emit sendAnswer(answer);
}

void SimpleQuestion::onIncorrectClicked()
{
    answer = false;
    emit sendAnswer(answer);
}
