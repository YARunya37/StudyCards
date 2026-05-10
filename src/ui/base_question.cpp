#include "base_question.h"

Question::Question(StudyCardWidget *referenceCard, QWidget *parent)
    : QStackedWidget(parent)
    , refCard(referenceCard)
    , face(new QWidget(this))
    , back(new QWidget(this))
    , answer(false)
{
    // Добавляем страницы в стек
    addWidget(face);  // index 0
    addWidget(back);  // index 1

    // По умолчанию показываем лицевую сторону
    setCurrentIndex(0);
}

void Question::ShowBack()
{
    setCurrentIndex(1);
}