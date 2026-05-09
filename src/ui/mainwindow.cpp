#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/FileLoader.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QLabel>

#include "filetreewidget.h"
#include "scaledtextedit.h"
#include "textformattingtoolbar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
     ui->setupUi(this);

    // Создаём FileTreeWidget
    FileTreeWidget* sourceTree = new FileTreeWidget(this);
    ui->horizontalLayout->addWidget(sourceTree);

    // Создаём toolbar как отдельный виджет:
    textToolbar = new TextFormattingToolbar(this);

    // Создаём разделитель
    SetUpSPlitter();

    // Подключаем кнопку добавления файла
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

    // Создаём контейнер для toolbar + текст
    toolbarPanel = new QWidget(this);
    QVBoxLayout* containerLayout = new QVBoxLayout(toolbarPanel);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(5);

    // Добавляем toolbar
    containerLayout->addWidget(textToolbar);

    // Текстовый редактор
    sourceTextWidget = new ScaledTextEdit(this);
    sourceTextWidget->setPlainText("Добавьте файл с помощью кнопки в панели");
    // Перенос слов
    sourceTextWidget->setLineWrapMode(QTextEdit::WidgetWidth);
    sourceTextWidget->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    containerLayout->addWidget(sourceTextWidget);

    // Подключаем редактор к toolbar
    textToolbar->setActiveEditor(sourceTextWidget);

    splitter->addWidget(toolbarPanel);

    // Создаём контейнер для label + tabWidget
    QWidget* tabContainer = new QWidget(this);
    QVBoxLayout* tabLayout = new QVBoxLayout(tabContainer);
    tabLayout->setContentsMargins(0, 0, 1, 0);
    tabLayout->setSpacing(0);

    // Label для названия группы билетов
    QLabel* groupLabel = new QLabel("Билеты по предмету", this);
    groupLabel->setFont(QFont("Segoe UI", 12, QFont::Bold));
    groupLabel->setAlignment(Qt::AlignCenter);
    groupLabel->setStyleSheet(
                "QLabel { "
                "    padding: 8px; "
                "    background: #3b3b3b; "
                "    color: #ffffff; "
                "    border-radius: 4px; "
                "    font-weight: bold; "
                "}"
                );
    tabLayout->addWidget(groupLabel);

    // Сохраняем указатель для последующего изменения текста
    ticketGroupLabel = groupLabel;

    tabLayout->addWidget(ui->tabWidget);
    splitter->addWidget(tabContainer);
    // Виджет с билетами можно убрать разделителем
    splitter->setCollapsible(splitter->indexOf(tabContainer), true);
    // Убираем расстояние между окнами
    splitter->setHandleWidth(0);
}

void MainWindow::setTicketGroupName(const QString& name)
{
    if (ticketGroupLabel) {
        ticketGroupLabel->setText(name);
    }
}
