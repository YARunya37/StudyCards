#include "testwindow.h"

TestWindow::TestWindow(Group* refGroup, QWidget *parent)
    : QWidget(parent)
    , refGroup(refGroup)
    , m_testStarted(false)
{
    // НАСТРАИВАЕМ TestDirector
    testDirector = new TestDirector(refGroup, this);

    connect(testDirector, &TestDirector::answerReceived,
            this, &TestWindow::onAnswerReceived);
    connect(testDirector, &TestDirector::testFinished,
            this, &TestWindow::onTestFinished);


    // НАСТРАИВАЕМ ОКНО

    setWindowFlags(Qt::Window);  // Делаем отдельным окном
    setWindowTitle("Тест: " + refGroup->Name());

    // Устанавливаем белый фон
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::white);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    setupUI();

    // Показываем стартовый экран
    showStartScreen();
}

TestWindow::~TestWindow()
{
}

void TestWindow::setupUI()
{
    mainLayout = new QVBoxLayout(this);

    // === ЗАГОЛОВОК ===
    headerWidget = new QWidget(this);
    QVBoxLayout* headerLayout = new QVBoxLayout(headerWidget);

    // Название группы
    groupNameLabel = new QLabel(refGroup->Name(), headerWidget);


    // Стили
    groupNameLabel->setAlignment(Qt::AlignCenter);
    QFont groupFont = groupNameLabel->font();
    groupFont.setPointSize(16);
    groupFont.setBold(true);
    groupNameLabel->setFont(groupFont);

    // Количество вопросов
    questionCountLabel = new QLabel(headerWidget);


    // Стили
    questionCountLabel->setAlignment(Qt::AlignCenter);
    QFont questionCountFont = questionCountLabel->font();
    questionCountFont.setPointSize(12);
    questionCountLabel->setFont(questionCountFont);


    headerLayout->addWidget(groupNameLabel);
    headerLayout->addWidget(questionCountLabel);


    // === КОНТЕЙНЕР ДЛЯ КОНТЕНТА ===
    contentContainer = new QWidget(this);
    contentLayout = new QVBoxLayout(contentContainer);
    contentLayout->setContentsMargins(0, 0, 0, 0);


    // === КНОПКА ===
    m_actionButton = new QPushButton("Начать тест", this);

    m_actionButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #E3F2FD;"      // Очень светло-синий фон
        "    color: #1976D2;"                 // Темно-синий текст
        "    border: 2px solid #2196F3;"      // Синяя рамка
        "    border-radius: 6px;"
        "    padding: 10px 20px;"
        "    font-size: 14px;"
        "    font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "    background-color: #BBDEFB;"      // Чуть темнее при наведении
        "    border: 2px solid #1976D2;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #90CAF9;"
        "    color: #0D47A1;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #F5F5F5;"
        "    color: #BDBDBD;"
        "    border: 2px solid #E0E0E0;"
        "}"
        );

    // Стили
    m_actionButton->setMinimumHeight(40);
    QFont buttonFont = m_actionButton->font();
    buttonFont.setPointSize(12);
    m_actionButton->setFont(buttonFont);

    // Подключение
    connect(m_actionButton, &QPushButton::clicked,
            this, &TestWindow::onStartButtonClicked);

    // Собираем всё вместе
    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(contentContainer, 1);
    mainLayout->addWidget(m_actionButton);


    updateHeaderInfo();
}

void TestWindow::showStartScreen()
{
    // Удаляем текущий виджет если есть
    if (currentQuestionWidget) {
        contentLayout->removeWidget(currentQuestionWidget);
        currentQuestionWidget->setParent(nullptr);
        currentQuestionWidget = nullptr;
    }

    // Создаем стартовый экран
    QWidget* startWidget = new QWidget(contentContainer);
    QVBoxLayout* startLayout = new QVBoxLayout(startWidget);

    // Вопрос-подтверждение
    message = new QLabel("Начать тест с группой " + groupNameLabel->text().toLower() + "?", startWidget);
    message->setAlignment(Qt::AlignCenter);
    QFont startFont = message->font();
    startFont.setPointSize(18);
    message->setFont(startFont);

    // Ставим надписть посередине layout
    startLayout->addStretch();
    startLayout->addWidget(message);
    startLayout->addStretch();

    // Добавляем в контейнер
    contentLayout->addWidget(startWidget);
    currentQuestionWidget = startWidget;
}

void TestWindow::showQuestion(Question* question)
{
    if (!question) {
        return;
    }

    // Удаляем текущий виджет
    if (currentQuestionWidget) {
        contentLayout->removeWidget(currentQuestionWidget);
        currentQuestionWidget = nullptr;
    }

    // Показываем вопрос
    contentLayout->addWidget(question);
    currentQuestionWidget = question;
}

void TestWindow::showFinishScreen()
{
    // Удаляем текущий виджет
    if (currentQuestionWidget) {
        contentLayout->removeWidget(currentQuestionWidget);
        // currentQuestionWidget->setParent(nullptr);
        currentQuestionWidget = nullptr;
    }
    // Создаем финальный экран
    QWidget* finishWidget = new QWidget(contentContainer);
    QVBoxLayout* finishLayout = new QVBoxLayout(finishWidget);

    QLabel* finishLabel = new QLabel(
        QString("Тест завершен!\n\n"),
        finishWidget
        );
    finishLabel->setAlignment(Qt::AlignCenter);
    QFont finishFont = finishLabel->font();
    finishFont.setPointSize(18);
    finishLabel->setFont(finishFont);

    finishLayout->addStretch();
    finishLayout->addWidget(finishLabel);
    finishLayout->addStretch();

    contentLayout->addWidget(finishWidget);
    currentQuestionWidget = finishWidget;
}

void TestWindow::updateHeaderInfo()
{
    if (!m_testStarted) {
        questionCountLabel->setText(QString("Вопросов: %1").arg(refGroup->GetAllCards().size()));
    }
}

void TestWindow::onStartButtonClicked()
{
    delete message;
    m_testStarted = true;

    testDirector->StartTest();

    Question* firstQuestion = testDirector->GetCurrentQuestion();
    if (firstQuestion) {
        showQuestion(firstQuestion);
    }

    m_actionButton->setText("Следующий вопрос");
    m_actionButton->setEnabled(false);

    disconnect(m_actionButton, &QPushButton::clicked,
               this, &TestWindow::onStartButtonClicked);
    connect(m_actionButton, &QPushButton::clicked,
            this, &TestWindow::onNextButtonClicked);

    updateHeaderInfo();
}

void TestWindow::onNextButtonClicked()
{
    m_actionButton->setEnabled(false);

    testDirector->NextQuestion();

    Question* nextQuestion = testDirector->GetCurrentQuestion();
    if (nextQuestion) {
        showQuestion(nextQuestion);
    }
}

void TestWindow::onAnswerReceived()
{
    m_actionButton->setEnabled(true);

    updateHeaderInfo();
}

void TestWindow::onTestFinished()
{
    showFinishScreen();

    m_actionButton->setText("Закрыть");
    m_actionButton->setEnabled(true);

    disconnect(m_actionButton, &QPushButton::clicked,
               this, &TestWindow::onNextButtonClicked);
    connect(m_actionButton, &QPushButton::clicked,
            this, &TestWindow::close);
}
