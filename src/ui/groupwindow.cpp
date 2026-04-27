#include "groupwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QListWidget>
GroupWindow::GroupWindow(Group* group, QWidget *parent)
    : QMainWindow{parent}, group{group}
{
    setupUI();
}

void GroupWindow::setupUI()
{
    setWindowTitle("Группа:" + group->Name());
    setMinimumSize(800, 600);

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Панель заголовка
    QWidget *headerWidget = new QWidget(this);
    headerWidget->setStyleSheet("background-color: #2c3e50; padding: 10px;");
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);

    QLabel* titleLabel = new QLabel(group->Name(), headerWidget);
    titleLabel->setStyleSheet(
        "color: white; "
        "font-size: 18px; "
        "font-weight: bold;"
        );
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();


    // Splitter для разделения списка и деталей
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    // Левая панель - список элементов
    QWidget *leftPanel = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(5, 5, 5, 5);

    QLabel *listLabel = new QLabel("Элементы:", leftPanel);
    listLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    QListWidget* itemsList = new QListWidget(leftPanel);
    itemsList->setStyleSheet(
        "QListWidget { "
        "   border: 1px solid #bdc3c7; "
        "   border-radius: 5px; "
        "   padding: 5px; "
        "} "
        "QListWidget::item { "
        "   padding: 8px; "
        "   border-bottom: 1px solid #ecf0f1; "
        "} "
        "QListWidget::item:selected { "
        "   background-color: #3498db; "
        "   color: white; "
        "}"
        );

    leftLayout->addWidget(listLabel);
    leftLayout->addWidget(itemsList);


    // Добавление в splitter
    splitter->addWidget(leftPanel);
    // splitter->addWidget(detailsWidget);
    splitter->setStretchFactor(0, 1); // Левая панель - 1 часть
    splitter->setStretchFactor(1, 2); // Правая панель - 2 части

    // Сборка основного layout
    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(splitter);

    setCentralWidget(centralWidget);
}
