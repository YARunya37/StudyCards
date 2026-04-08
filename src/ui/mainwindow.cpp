#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Создаём разделитель
    SetUpSPlitter(parent);

    // Подключаем реализацию функции добавления файла к кнопке
    connect(ui->add_file, &QAction::triggered, this, &MainWindow::AddFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::AddFile()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "Выбрать файлы",
        "",
        "Текстовые документы (*.docx *.md *.pdf)"
    );
    foreach (auto file, files) {
        qInfo() << file << "\n";
    }

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
