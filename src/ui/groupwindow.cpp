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
        setCard(item->text());
    });

    // Настройка собственного меню
    cardList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(cardList, &QListWidget::customContextMenuRequested,
            this, &GroupWindow::showContextMenu);

    // Создаем шорткат для клавиши Delete
    QShortcut *deleteShortcut = new QShortcut(QKeySequence::Delete, cardList);
    connect(deleteShortcut, &QShortcut::activated, this, &GroupWindow::deleteItem);

    // Подгружаем все созданные билеты
    foreach (auto card_name, group->GetAllCards()) {
        cardList->addItem(card_name);

        // Подключаем изменение заголовка
        QListWidgetItem* item = cardList->findItems(card_name, Qt::MatchExactly).value(0);
        connect(group->GetCard(card_name), &StudyCardWidget::header_changed,
                this,
                [group, item](const QString& new_name){
                    if(new_name != ""){
                        group->RenameCard(item->text(), new_name);
                        item->setText(new_name);
                    }
                }
        );
    }

}

void GroupWindow::add_new_card()
{
    TextInputDialog dialog = TextInputDialog(this, "Создать билет", "Введите название билета:");

    // Запускаем диалог. Если пользователь принимает имя, то отправляем его на создание
    if (dialog.exec() == QDialog::Accepted)
    {
        QString card_name = dialog.getText();
        // Если не удалось создать группу выводим сообщение об ошибку
        if(!group->CreateCard(card_name)){
            QMessageBox::warning(
                qobject_cast<QWidget*>(this->parent()),                          // parent
                "Ошибка создания группы",                // заголовок
                "Билет с именем \"" + card_name + "\" уже существует.\n"
                                                    "Пожалуйста, выберите другое имя.",     // текст
                QMessageBox::Ok                          // кнопки
                );
        }
        else{
            try{
                // Иначе устанавливаем новый билет активным
                setCard(card_name);

                cardList->addItem(card_name);

                // Подключаем возможность менять имя билета
                QListWidgetItem* new_item = cardList->findItems(card_name, Qt::MatchExactly).value(0);
                connect(active_card, &StudyCardWidget::header_changed,
                        this,
                        [this, new_item](const QString& new_name){
                            if(new_name != ""){
                                group->RenameCard(new_item->text(), new_name);
                                new_item->setText(new_name);
                            }
                        }
                        );
            }catch(const std::invalid_argument& e){
                QMessageBox::warning(
                    qobject_cast<QWidget*>(this->parent()),                          // parent
                    "Ошибка создания группы",                // заголовок
                    e.what(),     // текст
                    QMessageBox::Ok                          // кнопки
                    );
            }
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
    // Создаём окно подтверждения
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Подтверждение");
    msgBox.setText("Вы уверены, что хотите удалить билет \"" + item->text() + "\"?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);


    // Переименовываем кнопки
    msgBox.button(QMessageBox::Yes)->setText("Да");
    msgBox.button(QMessageBox::No)->setText("Нет");

    bool isActiveCard = false;
    if(group->GetCard(item->text()) == active_card){
        isActiveCard = true;
    }
    if (msgBox.exec() == QMessageBox::Yes && group->DeleteCard(item->text())) {
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
    verticalLayout->addWidget(create_button_panel());
    verticalLayout->addWidget(new TextFormattingToolbar(this));

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

void GroupWindow::setCard(const QString& card_name)
{
    // Убираем предыдущий билет
    if(active_card){
        active_card->hide();
        content->layout()->removeWidget(active_card);
        active_card = nullptr;
    }
    // Выбираем отображемый билет и устанавливаем его
    auto new_card = group->GetCard(card_name);
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
    panel->setFixedHeight(35); // Фиксированная высота панели

    panel->setStyleSheet(
        "QFrame {"
        "   background-color: #e0e0e0;"  // Светло-серый фон панели
        "   border: 1px solid #bdbdbd;"
        "}"
        );

    QHBoxLayout *layout = new QHBoxLayout(panel);
    layout->setContentsMargins(0, 2, 0, 2); // Отступы: left, top, right, bottom
    layout->setSpacing(2); // Расстояние между кнопками

    // Кнопка добавления с плюсиком
    QPushButton *addButton = new QPushButton("+");
    addButton->setFixedHeight(30);
    addButton->setFixedWidth(30); // Квадратная кнопка
    addButton->setStyleSheet(
        "QPushButton {"
            "   font-size: 20px;"
            "   font-weight: bold;"
            "   background-color: #d0d0d0;"  // На оттенок темнее панели
            "   border: 1px solid #bdbdbd;"
            "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
            "   background-color: #c0c0c0;"  // Ещё темнее при наведении
        "}"
        "QPushButton:pressed {"
            "   background-color: #b0b0b0;"
        "}"
    );

    // Кнопка запуска тестирования
    QPushButton *testButton = new QPushButton("Запустить тестирование");
    testButton->setFixedHeight(30);
    testButton->setMinimumWidth(200); // Минимальная ширина
    testButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #d0d0d0;"
        "   border: 1px solid #bdbdbd;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #c0c0c0;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #b0b0b0;"
        "}"
        );

    layout->addWidget(addButton);
    layout->addWidget(testButton);
    layout->addStretch(); // Добавляет пространство справа

    // Подключаем кнопки
    connect(addButton, &QPushButton::clicked, this, &GroupWindow::add_new_card);
    connect(testButton, &QPushButton::clicked, this, [this](){
        TestWindow* testWindow = new TestWindow(this->group, this);
        testWindow->resize(1000, 800);
        testWindow->show();
    });

    return panel;
}
