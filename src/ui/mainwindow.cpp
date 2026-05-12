#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QLabel>

#include "filetreewidget.h"
#include "textformattingtoolbar.h"
#include "filemanager.h"
#include "documentui.h"
#include "groupsuicontroller.h"

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

    // Создаём менеджер документов:
    FileManager* fileManager = sourceTree->getFileManager();

    // Создаём DocumentUI и подключаемсохранение:
    DocumentUI* docUI = new DocumentUI(this, sourceTree, sourceTextWidget, fileManager, this);

    docUI->connectMenuActions(ui->action_save);

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
    // Открытие группы в окне
    connect(ui->open_group_in_window, &QAction::triggered, controller, &GroupsUIController::choose_group_to_open);
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
    sourceTextWidget = new QTextEdit(this);
    sourceTextWidget->setPlainText("Добавьте файл с помощью кнопки в панели");
    sourceTextWidget->setReadOnly(true);
    // Перенос слов
    sourceTextWidget->setLineWrapMode(QTextEdit::WidgetWidth);
    sourceTextWidget->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    sourceTextWidget->setTextInteractionFlags(Qt::TextEditorInteraction | Qt::LinksAccessibleByMouse);
    containerLayout->addWidget(sourceTextWidget);

    // Настройка шрифта
    QFont baseFont = sourceTextWidget->font();
    baseFont.setPointSize(12);
    sourceTextWidget->setFont(baseFont);
    sourceTextWidget->document()->setDefaultFont(baseFont);

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
