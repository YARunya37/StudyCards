#include "tempcardswidget.h"
#include "studycard.h"
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
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
    else{
        setupInitUI();
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

    // Кнопка "Отклонить"
    reject_button->setStyleSheet(
        "QPushButton {"
        "    background-color: white;"
        "    color: #1a1a1a;"
        "    border: 1px solid #d0d0d0;"
        "    padding: 6px 12px;"
        "    border-radius: 6px;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #f8f8f8;"
        "    border: 1px solid #b0b0b0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #f0f0f0;"
        "    border: 1px solid #a0a0a0;"
        "}"
        "QPushButton:checked {"
        "    background-color: white;"
        "    border: 2px solid #1a1a1a;"
        "    font-weight: 600;"
        "}"
        );

    // Кнопка "Добавить в группу"
    add_button->setStyleSheet(
        "QPushButton {"
        "    background-color: white;"
        "    color: #1a1a1a;"
        "    border: 1px solid #d0d0d0;"
        "    padding: 6px 12px;"
        "    border-radius: 6px;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #f8f8f8;"
        "    border: 1px solid #b0b0b0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #f0f0f0;"
        "    border: 1px solid #a0a0a0;"
        "}"
        "QPushButton:checked {"
        "    background-color: white;"
        "    border: 2px solid #1a1a1a;"
        "    font-weight: 600;"
        "}"
        );

    // Кнопка "Создать"
    create_button->setStyleSheet(
        "QPushButton {"
        "    background-color: white;"
        "    color: #1a1a1a;"
        "    border: 1px solid #d0d0d0;"
        "    padding: 6px 12px;"
        "    border-radius: 6px;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #f8f8f8;"
        "    border: 1px solid #b0b0b0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #f0f0f0;"
        "    border: 1px solid #a0a0a0;"
        "}"
        "QPushButton:checked {"
        "    background-color: white;"
        "    border: 2px solid #1a1a1a;"
        "    font-weight: 600;"
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
        QMessageBox::information(this,
                                 "Билет добавлен",
                                 QString("Билет успешно добавлен в группу \"%1\".\n\n"
                                         "Вы можете посмотреть все билеты в окне конкретной группы.\n"
                                         "Для его открытия нажмите:\n"
                                         "Группы билетов → Открыть группу в окне")
                                     .arg(curr_group->Name()));

        return true;
    }

    QMessageBox::warning(this,
                         "Ошибка",
                         "Не удалось добавить билет в группу.\n"
                         "Пожалуйста, попробуйте снова.");

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
