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

    // Создаём контейнер для toolbar + текст
    midContentContainer = new QWidget(this);
    QVBoxLayout* containerLayout = new QVBoxLayout(midContentContainer );
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(5);

    // Добавляем toolbar
    TextFormattingToolbar* textToolbar = new TextFormattingToolbar(this);
    containerLayout->addWidget(textToolbar);

    // Текстовый редактор
    QTextEdit* sourceTextWidget = new QTextEdit(this);
    sourceTextWidget->setPlainText("Добавьте файл с помощью кнопки в панели");
    sourceTextWidget->setReadOnly(true);
    // Перенос слов
    sourceTextWidget->setLineWrapMode(QTextEdit::WidgetWidth);
    sourceTextWidget->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    sourceTextWidget->setAlignment(Qt::AlignCenter);

    containerLayout->addWidget(sourceTextWidget);


    // Создаём контейнер для label + tabWidget
    tempGroupContainer = new QWidget(this);
    QVBoxLayout* tempGroupLayout = new QVBoxLayout(tempGroupContainer);
    tempGroupLayout->setContentsMargins(0, 0, 1, 0);
    tempGroupLayout->setSpacing(0);

    // Label для названия группы билетов
    QLabel* groupLabel = new QLabel("Группа не выбрана", this);
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
    tempGroupLayout->addWidget(groupLabel);
    tempGroupLayout->addWidget(ui->tabWidget);
    // Сохраняем указатель для последующего изменения текста
    ticketGroupLabel = groupLabel;

    // Создаём разделители
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
    // Подключаем при смене активной группы изменение label с выбранной группой
    connect(controller, &GroupsUIController::active_group_changed, this, [this](Group* new_active_group){
        if(new_active_group){
            setTicketGroupName(new_active_group->Name());
        }
    });
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
    splitter->addWidget(midContentContainer);
    splitter->addWidget(tempGroupContainer);


    // Виджет с билетами можно убрать разделителем
    splitter->setCollapsible(splitter->indexOf(tempGroupContainer), true);
    // Убираем расстояние между окнами
    splitter->setHandleWidth(0);
}

void MainWindow::setTicketGroupName(const QString& name)
{
    if (ticketGroupLabel) {
        ticketGroupLabel->setText(name);
    }
}
