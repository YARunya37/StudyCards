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

    // Добавляем с растяжением
    buttons_layout->addStretch();
    buttons_layout->addWidget(reject_button);
    buttons_layout->addWidget(add_button);
    buttons_layout->addWidget(create_button);
    buttons_layout->addStretch();

    // Подключение кнопок

    connect(add_button, &QPushButton::clicked, this, &TempCardsWidget::addToGroup);
    connect(create_button, &QPushButton::clicked, this, &TempCardsWidget::createEmptyPage);

    // Добавляем горизонтальный layout в вертикальный
    page_layout->addLayout(buttons_layout);

    addTab(page_content, "Вопрос " + QString::number(this->count()+1));
}

bool TempCardsWidget::addToGroup()
{
    auto card = this->currentWidget()->findChild<StudyCardWidget*>();
    if(card && curr_group->CreateCard(card)){
        card->hide();
        if(this->count() == 1){
            setupTempCardUI();
        }
        else{
            removeTab(this->currentIndex());
        }
        return true;
    }
    return false;
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
