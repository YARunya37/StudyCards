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
    questionText->setAlignment(Qt::AlignCenter);

    // Кнопка показать ответ
    showAnswerButton = new QPushButton("Показать ответ", face);
    connect(showAnswerButton, &QPushButton::clicked, this, &SimpleQuestion::onShowAnswerClicked);

    showAnswerButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2196F3;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    padding: 4px 10px;"
        "    font-size: 11px;"
        "    font-weight: bold;"
        "    min-height: 25px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #1976D2;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0D47A1;"
        "}"
        );


    // Добавляем в layout
    faceLayout->addWidget(questionText);
    faceLayout->addWidget(showAnswerButton);
}

void SimpleQuestion::setup_back()
{
    // Создаем layout для обратной стороны
    QVBoxLayout *backLayout = new QVBoxLayout(back);
    backLayout->setContentsMargins(20, 20, 20, 20);  // добавьте отступы
    backLayout->setSpacing(15);

    // Текст ответа
    answerText = new QTextEdit(back);
    answerText->setReadOnly(true);

    // Оборачиваем HTML-контент в div с чёрным цветом и белым фоном
        QString bodyContent = refCard->GetBodyContent();
        bodyContent = QString(
            "<div style='color: black; background-color: white; padding: 16px;'>"
            "%1"
            "</div>"
        ).arg(bodyContent);
        answerText->setHtml(bodyContent);

        // Добавляем рамку через styleSheet
            answerText->setStyleSheet(
                "QTextEdit {"
                "    border: 1px solid #E0E0E0;"
                "    border-radius: 8px;"
                "    background-color: white;"
                "}"
            );

    // КНОПКА "ПОКАЗАТЬ ВОПРОС" (ПЕРЕВОРОТ)
    QPushButton* flipButton = new QPushButton("🔄 Показать вопрос", back);
        flipButton->setStyleSheet(
            "QPushButton {"
            "    background-color: #2196F3;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 4px;"
            "    padding: 6px 12px;"
            "    font-size: 11px;"
            "    font-weight: bold;"
            "    min-height: 30px;"
            "}"
            "QPushButton:hover { background-color: #1976D2; }"
            "QPushButton:pressed { background-color: #0D47A1; }"
        );
    connect(flipButton, &QPushButton::clicked, this, [this](){
        ShowFace();  // Показываем лицевую сторону (вопрос)
    });

    // Кнопки верно/неверно
    correctButton = new QPushButton("Верно", back);
    incorrectButton = new QPushButton("Неверно", back);

    // Кнопка "Верно" - очень маленькая
    correctButton->setStyleSheet(
        "QPushButton {"
        "    background-color: white;"
        "    color: #28A745;"
        "    border: 2px solid #28A745;"
        "    border-radius: 4px;"             // Уменьшено с 6px
        "    padding: 4px 10px;"              // Уменьшено с 8px 16px
        "    font-size: 11px;"                // Уменьшено с 13px
        "    font-weight: bold;"
        "    min-height: 25px;"               // Уменьшено с 35px
        "}"
        "QPushButton:hover {"
        "    background-color: #28A745;"
        "    color: white;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #218838;"
        "    border-color: #218838;"
        "    color: white;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #E0E0E0;"   // Серый фон когда отключена
        "    color: #9E9E9E;"
        "    border-color: #BDBDBD;"
        "}"
        );

    // Кнопка "Неверно" - очень маленькая
    incorrectButton->setStyleSheet(
        "QPushButton {"
        "    background-color: white;"
        "    color: #DC3545;"
        "    border: 2px solid #DC3545;"
        "    border-radius: 4px;"             // Уменьшено с 6px
        "    padding: 4px 10px;"              // Уменьшено с 8px 16px
        "    font-size: 11px;"                // Уменьшено с 13px
        "    font-weight: bold;"
        "    min-height: 25px;"               // Уменьшено с 35px
        "}"
        "QPushButton:hover {"
        "    background-color: #DC3545;"
        "    color: white;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #C82333;"
        "    border-color: #C82333;"
        "    color: white;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #E0E0E0;"   // Серый фон когда отключена
        "    color: #9E9E9E;"
        "    border-color: #BDBDBD;"
        "}"
        );

    connect(correctButton, &QPushButton::clicked, this, &SimpleQuestion::onCorrectClicked);
    connect(incorrectButton, &QPushButton::clicked, this, &SimpleQuestion::onIncorrectClicked);

    // Layout для кнопок Верно/Неверно
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(correctButton);
    buttonsLayout->addWidget(incorrectButton);

    // Layout для кнопки "Показать вопрос" и пары Верно/Неверно
    QVBoxLayout *bottomLayout = new QVBoxLayout();
    bottomLayout->addWidget(flipButton);
    bottomLayout->addLayout(buttonsLayout);

    // Добавляем в основной layout
    backLayout->addWidget(answerText, 1);  // растягивается
    backLayout->addLayout(bottomLayout);   // прижат к низу
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

    // Отключаем обе кнопки
    correctButton->setEnabled(false);
    incorrectButton->setEnabled(false);

    // Подсвечиваем выбранную кнопку
    correctButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #28A745;"   // Зеленый фон
        "    color: white;"
        "    border: 2px solid #28A745;"
        "    border-radius: 4px;"
        "    padding: 4px 10px;"
        "    font-size: 11px;"
        "    font-weight: bold;"
        "    min-height: 25px;"
        "}"
        );

    emit sendAnswer(answer);
}

void SimpleQuestion::onIncorrectClicked()
{
    answer = false;

    // Отключаем обе кнопки
    correctButton->setEnabled(false);
    incorrectButton->setEnabled(false);

    // Подсвечиваем выбранную кнопку (красным)
    incorrectButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #DC3545;"   // Красный фон
        "    color: white;"
        "    border: 2px solid #DC3545;"
        "    border-radius: 4px;"
        "    padding: 4px 10px;"
        "    font-size: 11px;"
        "    font-weight: bold;"
        "    min-height: 25px;"
        "}"
        );

    emit sendAnswer(answer);
}
