#include "testgenerator.h"

TestGenerator::TestGenerator(Group* group) {
    if(group){
        init_questions(group);
    }
}

QList<Question *> TestGenerator::GetQuestions()
{
    return questions;
}

void TestGenerator::init_questions(Group *group)
{
    // Перебираем все билеты в группе
    foreach (auto card_name, group->GetAllCards()) {
        StudyCardWidget* card = group->GetCard(card_name);

        // Создаём вопрос. НА ДАННЫЙ МОМЕНТ СОЗДАЁТСЯ ПРОСТОЙ ВОПРОС И ВСЁ, ДАЛЕЕ НУЖНО БУДЕТ ВЫНОСИТЬ ОПРЕДЕЛЕНИЕ ТИПА ВОПРОСА
        auto question = create_simple_question(card);
        if(question){
            questions.append(question);
        }
    }
}

SimpleQuestion *TestGenerator::create_simple_question(StudyCardWidget *card)
{
    if(!card)   return nullptr;

    SimpleQuestion* question = new SimpleQuestion(card, nullptr);

    return question;
}
