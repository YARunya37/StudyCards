#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Создаём разделитель
    SetUpSPlitter(parent);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetUpSPlitter(QWidget* parent)
{
    // Создание разделителя между виджетами, для изменения их размеров
    QSplitter* splitter = new QSplitter(parent);
    ui->horizontalLayout->addWidget(splitter);
    // Запрещаем окнам пропадать, если пользователь их сильно уменьшает
    splitter->setChildrenCollapsible(false);
    // Добавляем разделители
    splitter->addWidget(ui->sourceTree);
    splitter->addWidget(ui->sourceTextWidget);
    splitter->addWidget(ui->tabWidget);
    // Виджет с билетами можно убрать разделителем
    splitter->setCollapsible(splitter->indexOf(ui->tabWidget), true);
    // Убираем расстояние между окнами
    splitter->setHandleWidth(0);
}
