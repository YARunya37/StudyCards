#include "testdirector.h"
#include <QRandomGenerator>
#include <algorithm>

TestDirector::TestDirector(Group* refGroup, QObject *parent)
    : QObject(parent)
    , m_refGroup(refGroup)
    , m_testGenerator(nullptr)
    , m_currentQuestion(nullptr)
{
}

TestDirector::~TestDirector()
{
    clearQueue();

    if (m_currentQuestion) {
        delete m_currentQuestion;
        m_currentQuestion = nullptr;
    }

    if (m_testGenerator) {
        delete m_testGenerator;
        m_testGenerator = nullptr;
    }
}

void TestDirector::StartTest()
{
    // Очищаем предыдущую очередь
    clearQueue();

    if (m_currentQuestion) {
        delete m_currentQuestion;
        m_currentQuestion = nullptr;
    }

    // Создаем генератор тестов
    if (!m_testGenerator) {
        m_testGenerator = new TestGenerator(m_refGroup);
    }

    // Получаем вопросы
    QList<Question*> questions = m_testGenerator->GetQuestions();

    if (questions.isEmpty()) {
        return;
    }

    // Перемешиваем вопросы в случайном порядке
    std::shuffle(questions.begin(), questions.end(),
                 std::default_random_engine(QRandomGenerator::global()->generate()));

    // Заполняем очередь
    foreach (Question* question, questions) {
        m_questionQueue.enqueue(question);

        // Подключаем сигнал ответа от вопроса к слоту директора
        connect(question, &Question::sendAnswer,
                this, &TestDirector::onAnswerReceived);
    }

    // Показываем первый вопрос
    NextQuestion();
}

void TestDirector::NextQuestion()
{
    // Удаляем предыдущий вопрос
    if (m_currentQuestion) {
        delete m_currentQuestion;
        m_currentQuestion = nullptr;
    }

    // Проверяем, есть ли вопросы в очереди
    if (m_questionQueue.isEmpty()) {
        emit testFinished();
        return;
    }

    // Берем следующий вопрос из очереди
    m_currentQuestion = m_questionQueue.dequeue();
}

void TestDirector::onAnswerReceived(bool isCorrect)
{
    // Отправляем сигнал о том, что ответ получен
    emit answerReceived();

    // Если ответ неправильный, копируем вопрос в конец очереди
    if (!isCorrect && m_currentQuestion) {
        // Создаем копию текущего вопроса
        Question* questionCopy = m_currentQuestion->Clone();

        if (questionCopy) {
            // Подключаем сигнал ответа
            connect(questionCopy, &Question::sendAnswer,
                    this, &TestDirector::onAnswerReceived);

            // Добавляем в конец очереди
            m_questionQueue.enqueue(questionCopy);
        }
    }
}

Question* TestDirector::GetCurrentQuestion() const
{
    return m_currentQuestion;
}

void TestDirector::clearQueue()
{
    while (!m_questionQueue.isEmpty()) {
        Question* question = m_questionQueue.dequeue();
        delete question;
    }
}
