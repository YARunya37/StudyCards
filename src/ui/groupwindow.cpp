#include "groupwindow.h"
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QFrame>
#include <QCoreApplication>
#include <QMenu>
#include "textinputdialog.h"
#include <QMessageBox>
#include <QShortcut>
#include "testwindow.h"
GroupWindow::GroupWindow(Group* group, QWidget *parent)
    : QMainWindow{parent}, group{group}
{
    setupUI();

    // НАСТРОЙКА СПИСКА БИЛЕТОВ
    connect(cardList, &QListWidget::itemDoubleClicked, this, [this](const QListWidgetItem* item){
        setCard(item->data(Qt::UserRole).toString());
    });

    // Настройка собственного меню
    cardList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(cardList, &QListWidget::customContextMenuRequested,
            this, &GroupWindow::showContextMenu);

    // Создаем шорткат для клавиши Delete
    QShortcut *deleteShortcut = new QShortcut(QKeySequence::Delete, cardList);
    connect(deleteShortcut, &QShortcut::activated, this, &GroupWindow::deleteItem);

    // Подгружаем все созданные билеты
    foreach(auto card_id, group->GetAllCards()){
        auto card = group->GetCard(card_id);
        if(!card) continue;

        QString question_text = card->GetQuestionTextFromFile().simplified();
        if (question_text.isEmpty() || question_text == "Ошибка чтения файла")
            question_text = card->GetQuestionText().simplified();
        if(question_text.isEmpty())
            question_text = "Билет " + card_id;

        QListWidgetItem* item = new QListWidgetItem(question_text);
        item->setData(Qt::UserRole, card_id);   // сохраняем ID
        cardList->addItem(item);

        connect(card, &StudyCardWidget::header_changed,
            this,
            [this, item, card_id](const QString& new_name) {
                QString cleaned = new_name.simplified();
                if (!cleaned.isEmpty() && cleaned != item->text()) {
                    item->setText(cleaned);
                }
            }
        );
    }
    // Автоматически выбираем первый билет
    QStringList cards = group->GetAllCards();
    if(!cards.isEmpty()){
        QString firstCardName = cards.first();
        cardList->setCurrentRow(0);
        try {
            setCard(firstCardName);
        } catch (...) {
            qWarning() << "Failed to load first card:" << firstCardName;
        }
    }
}

void GroupWindow::add_new_card()
{
    TextInputDialog dialog(this, "Создать билет", "Введите текст вопроса:");
    if (dialog.exec() == QDialog::Accepted) {
        QString question_text = dialog.getText().simplified();
        if (question_text.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Текст вопроса не может быть пустым.");
            return;
        }
        // CreateCard возвращает ID
        QString card_id = group->CreateCard(question_text);

        if(card_id.isEmpty()){
            QMessageBox::warning(this, "Ошибка создания билета", "Не удалось создать билет.");
        } else {
            setCard(card_id); // Передаём ID

            QListWidgetItem* new_item = new QListWidgetItem(question_text);
            new_item->setData(Qt::UserRole, card_id); // Сохраняем ID
            cardList->addItem(new_item);
            cardList->setCurrentItem(new_item);

            connect(active_card, &StudyCardWidget::header_changed,
                this,
                [this, new_item, card_id](const QString& new_name) {
                    QString cleaned = new_name.simplified();
                    if (!cleaned.isEmpty() && cleaned != new_item->text()) {
                        new_item->setText(cleaned);
                    }
                }
            );
        }
    }
}

void GroupWindow::showContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = cardList->itemAt(pos);

    if (!item)
        return; // Клик был не на элементе

    // Создаем меню
    QMenu contextMenu(this);

    QAction *deleteAction = contextMenu.addAction("Удалить");

    // Показываем меню и получаем выбранное действие
    QAction *selectedAction = contextMenu.exec(cardList->mapToGlobal(pos));

    if (selectedAction == deleteAction)
    {
        deleteItem( );
    }
}

void GroupWindow::deleteItem()
{
    QListWidgetItem *item = cardList->currentItem();
    if(!item) return;
    QString card_id = item->data(Qt::UserRole).toString();
    if(card_id.isEmpty()) return;

    // Создаём окно подтверждения
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Подтверждение");
    msgBox.setText("Вы уверены, что хотите удалить билет \"" + item->text() + "\"?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);


    // Переименовываем кнопки
    msgBox.button(QMessageBox::Yes)->setText("Да");
    msgBox.button(QMessageBox::No)->setText("Нет");

    bool isActiveCard = false;
    if(group->GetCard(card_id) == active_card){
        isActiveCard = true;
    }
    if (msgBox.exec() == QMessageBox::Yes && group->DeleteCard(card_id)) {
        if(isActiveCard){
            active_card = nullptr;
        }

        delete item;
    }
}

void GroupWindow::setupUI()
{
    setWindowTitle("Группа: " + group->Name());
    setMinimumSize(1000, 800);

    // Создаём центральный виджет
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout *verticalLayout = new QVBoxLayout(centralWidget);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->setSpacing(0);

    // Добавляем панель с кнопками
    verticalLayout->addWidget(new TextFormattingToolbar(this));
    verticalLayout->addWidget(create_button_panel());

    // Создаём виджет, в котором будет находиться весь контент
    content = new QWidget(this);
    verticalLayout->addWidget(content);

    QHBoxLayout* contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // Левая панель - список элементов
    cardList = new QListWidget();
    cardList->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    cardList->setStyleSheet(
        "QListWidget {"
        "   border: 1px solid #ccc;"
        "   border-radius: 4px;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #e3f2fd;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #2196F3;"
        "   color: white;"
        "}"
    );

    contentLayout->addWidget(cardList);


    this->setCentralWidget(centralWidget);
}

void GroupWindow::setCard(const QString& card_id)
{
    // Убираем предыдущий билет
    if(active_card){
        active_card->hide();
        content->layout()->removeWidget(active_card);
        active_card = nullptr;
    }
    // Выбираем отображемый билет и устанавливаем его
    auto new_card = group->GetCard(card_id);
    if(!new_card)
        throw std::invalid_argument("Card is unavaliable!");
    active_card = new_card;
    content->layout()->addWidget(active_card);
    active_card->show();
    active_card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QFrame* GroupWindow::create_button_panel()
{
    QFrame *panel = new QFrame();
    panel->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    panel->setFixedHeight(40);

    panel->setStyleSheet(
        "QFrame {"
        "    background-color: #f5f5f5;"
        "    border: none;"
        "    border-bottom: 1px solid #d0d0d0;"
        "}"
        );

    QHBoxLayout *layout = new QHBoxLayout(panel);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    // Кнопка добавления
    QPushButton *addButton = new QPushButton("+");
    addButton->setFixedSize(32, 32);
    addButton->setStyleSheet(
        "QPushButton {"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    background-color: white;"
        "    color: #1a1a1a;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #f8f8f8;"
        "    border: 1px solid #b0b0b0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #f0f0f0;"
        "}"
        );

    // Кнопка тестирования
    QPushButton *testButton = new QPushButton("Запустить тестирование");
    testButton->setFixedHeight(32);
    testButton->setMinimumWidth(180);
    testButton->setStyleSheet(
        "QPushButton {"
        "    background-color: white;"
        "    color: #1a1a1a;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 4px;"
        "    padding: 6px 12px;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #f8f8f8;"
        "    border: 1px solid #b0b0b0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #f0f0f0;"
        "}"
        );

    layout->addWidget(addButton);
    layout->addWidget(testButton);
    layout->addStretch();

    // Подключаем кнопки
    connect(addButton, &QPushButton::clicked, this, &GroupWindow::add_new_card);
    connect(testButton, &QPushButton::clicked, this, [this](){
        TestWindow* testWindow = new TestWindow(this->group, this);
        testWindow->resize(1000, 800);
        testWindow->show();
    });

    return panel;
}
