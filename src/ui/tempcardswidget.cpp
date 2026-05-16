#include "tempcardswidget.h"
#include "studycard.h"
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
TempCardsWidget::TempCardsWidget(QWidget* parent) :
    QTabWidget(parent)
{
    setupInitUI();
}

void TempCardsWidget::onActiveGroupChanged(Group *new_active_group)
{
    if(new_active_group){
        curr_group = new_active_group;
        setupTempCardUI();
    }
}

void TempCardsWidget::createEmptyPage()
{
    // Виджет пустого билета
    QWidget* page_content = new QWidget(this);

    QVBoxLayout* page_layout = new QVBoxLayout(page_content);
    page_layout->setContentsMargins(3, 5, 3, 0);

    auto empty_card = createEmptyCard(page_content);

    page_layout->addWidget(empty_card);

    // Горизонтальный layout для кнопок
    QHBoxLayout* buttons_layout = new QHBoxLayout();
    buttons_layout->setSpacing(10);  // Отступы между кнопками

    // Создаем три кнопки
    QPushButton* reject_button = new QPushButton("Отклонить", page_content);
    QPushButton* add_button = new QPushButton("Добавить в группу", page_content);
    QPushButton* create_button = new QPushButton("Создать", page_content);

    // Кнопка "Отклонить" - светло-серая с красным текстом
    reject_button->setStyleSheet(
        "QPushButton {"
        "    background-color: #f5f5f5;"
        "    color: #424242;"
        "    border: 1px solid #e0e0e0;"
        "    padding: 6px 12px;"
        "    border-radius: 4px;"
        "    font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #ffebee;"
        "    color: #b71c1c;"
        "    border: 2px solid #ffcdd2;"
        "}"
        );

    // Кнопка "Добавить в группу" - светло-серая с синим текстом
    add_button->setStyleSheet(
        "QPushButton {"
        "    background-color: #f5f5f5;"
        "    color: #424242;"
        "    border: 1px solid #e0e0e0;"
        "    padding: 6px 12px;"
        "    border-radius: 4px;"
        "    font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e3f2fd;"
        "    color: #0d47a1;"
        "    border: 1px solid #bbdefb;"
        "}"
        );

    // Кнопка "Создать" - светло-серая с зеленым текстом
    create_button->setStyleSheet(
        "QPushButton {"
        "    background-color: #f5f5f5;"
        "    color: #424242;"
        "    border: 1px solid #e0e0e0;"
        "    padding: 6px 12px;"
        "    border-radius: 4px;"
        "    font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e8f5e9;"
        "    color: #1b5e20;"
        "    border: 2px solid #c8e6c9;"
        "}"
        );


    // Добавляем с растяжением
    buttons_layout->addStretch();
    buttons_layout->addWidget(reject_button);
    buttons_layout->addWidget(add_button);
    buttons_layout->addWidget(create_button);
    buttons_layout->addStretch();

    // Подключение кнопок
    connect(reject_button, &QPushButton::clicked, this, &TempCardsWidget::rejectCard);
    connect(add_button, &QPushButton::clicked, this, &TempCardsWidget::addToGroup);
    connect(create_button, &QPushButton::clicked, this, &TempCardsWidget::createEmptyPage);

    // Добавляем горизонтальный layout в вертикальный
    page_layout->addLayout(buttons_layout);

    addTab(page_content, "Вопрос " + QString::number(this->count()+1));
}

bool TempCardsWidget::addToGroup()
{
    auto card = this->currentWidget()->findChild<StudyCardWidget*>();
    // Создаём в группе текущий билет
    if(card && curr_group->CreateCard(card)){
        // Прячем билет, чтобы вызвать сохранение текста перед удалением
        card->hide();

        // Удаляем вкладку
        remove_curr_tab();
        return true;
    }
    return false;
}

bool TempCardsWidget::rejectCard()
{
    auto card = this->currentWidget()->findChild<StudyCardWidget*>();
    // Удаляем вкладку с текущим билетом
    if(card){
        remove_curr_tab();
        return true;
    }
    else{
        return false;
    }
}

void TempCardsWidget::setupInitUI()
{
    QTextBrowser* hint = new QTextBrowser(this);
    hint->setText("Выберите группу через меню \"Группы билетов\"");
    hint->setAlignment(Qt::AlignCenter);

    QFont hintFont = QFont();
    hintFont.setPointSize(16);
    hint->setFont(hintFont);

    addTab(hint, "");
}

void TempCardsWidget::setupTempCardUI()
{
    // Удаляем вкладку по умолчанию
    removeTab(0);

    createEmptyPage();
}

StudyCardWidget *TempCardsWidget::createEmptyCard(QWidget* parent)
{
    StudyCardWidget* empty_card = new StudyCardWidget(parent, "/resources/usergroups/" + curr_group->Name(), "Вопрос " + QString::number(this->count()+1));
    return empty_card;
}

void TempCardsWidget::remove_curr_tab()
{
    // Если мы удаляем последнюю вкладку, то надо автоматически создать пустой начальный билет
    if(this->count() == 1){
        setupTempCardUI();
    }
    // Иначе можно просто удалить вкладку
    else{
        removeTab(this->currentIndex());
    }
}
