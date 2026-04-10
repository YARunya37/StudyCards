#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filetreewidget.h"
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

    // Подключаем реализацию функции добавления файла к кнопке
    connect(ui->add_file, &QAction::triggered, sourceTree, &FileTreeWidget::AddFiles);
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
