#ifndef TESTGENERATOR_H
#define TESTGENERATOR_H

#include "group.h"
#include "base_question.h"
#include "SimpleQuestion.h"
#include <QList>
class TestGenerator
{
public:
    TestGenerator(Group* group);

    // Получения списка вопросов
    QList<Question*> GetQuestions();
private:
    // Список созданных вопросов
    QList<Question*> questions;
    // Метод для создания списка вопросов
    void init_questions(Group* group);
    // Метод для создания обычного вопроса по билету
    SimpleQuestion* create_simple_question(StudyCardWidget* card);
};

#endif // TESTGENERATOR_H
