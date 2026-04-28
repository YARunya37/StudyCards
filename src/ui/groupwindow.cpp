#include "groupwindow.h"
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QFrame>
// #include <QSplitter>
#include <QListWidget>
GroupWindow::GroupWindow(Group* group, QWidget *parent)
    : QMainWindow{parent}, group{group}
{
    setupUI();
}

void GroupWindow::setupUI()
{
    setWindowTitle("Группа: " + group->Name());
    setMinimumSize(800, 600);

    // Создаём центральный виджет
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout *verticalLayout = new QVBoxLayout(centralWidget);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->setSpacing(0);

    // Добавляем панель с кнопками
    verticalLayout->addWidget(create_button_panel());


    // Создаём виджет, в котором будет находиться весь контент
    QWidget* content = new QWidget(this);
    verticalLayout->addWidget(content);

    QHBoxLayout* contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // Левая панель - список элементов
    QListWidget* cardList = new QListWidget();
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

    return panel;
}
