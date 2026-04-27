#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filetreewidget.h"
#include "groupsuicontroller.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FileTreeWidget* sourceTree = new FileTreeWidget(this);
    // Добавляем виджет дерева файлов
    ui->horizontalLayout->addWidget(sourceTree);
    // Создаём разделитель
    SetUpSPlitter();

    // Создаём контроллер групп
    GroupsUIController* controller = new GroupsUIController(this);

    // Подключаем реализацию функции добавления файла к кнопке
    connect(ui->add_file, &QAction::triggered, sourceTree, &FileTreeWidget::AddFiles);
    // Подключаем реализацию создания группы к кнопке в панели
    connect(ui->create_group, &QAction::triggered, controller, &GroupsUIController::show_creation_group_dialog);
    // Кнопка удаления группы
    connect(ui->delete_group, &QAction::triggered, controller, &GroupsUIController::show_delete_group_window);
    // Кнопка выбора группы
    connect(ui->select_group, &QAction::triggered, controller, &GroupsUIController::choose_active_group);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetUpSPlitter()
{
    // Создание разделителя между виджетами, для изменения их размеров
    QSplitter* splitter = new QSplitter(this);
    ui->horizontalLayout->addWidget(splitter);
    // Запрещаем окнам пропадать, если пользователь их сильно уменьшает
    splitter->setChildrenCollapsible(false);
    // Добавляем разделители
    splitter->addWidget(ui->horizontalLayout->parentWidget()->findChild<FileTreeWidget*>());
    splitter->addWidget(ui->sourceTextWidget);
    splitter->addWidget(ui->tabWidget);
    // Виджет с билетами можно убрать разделителем
    splitter->setCollapsible(splitter->indexOf(ui->tabWidget), true);
    // Убираем расстояние между окнами
    splitter->setHandleWidth(0);
}
