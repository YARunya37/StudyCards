#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/FileLoader.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QShortcut>
#include <QColorDialog>
#include <QComboBox>
#include <QLabel>

#include "filetreewidget.h"
#include "scaledtextedit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
     ui->setupUi(this);

    // Создаём FileTreeWidget
    FileTreeWidget* sourceTree = new FileTreeWidget(this);
    ui->horizontalLayout->addWidget(sourceTree);

    // Создаём контейнер и toolbar
    createToolbar();

    // Создаём разделитель
    SetUpSPlitter();

    // Подключаем кнопку добавления файла
    connect(ui->add_file, &QAction::triggered, sourceTree, &FileTreeWidget::AddFiles);
}


void MainWindow::createToolbar()
{
    // Контейнер для toolbar + текст
    textEditorContainer = new QWidget(this);
    QVBoxLayout* containerLayout = new QVBoxLayout(textEditorContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(5);

    // Toolbar (горизонтальная панель)
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setContentsMargins(5, 5, 5, 0);
    toolbarLayout->setSpacing(5);

    // Кнопка Bold
    btnBold = new QPushButton("B", this);
    btnBold->setMaximumWidth(40);
    btnBold->setFont(QFont("Segoe UI", 9, QFont::Bold));
    btnBold->setToolTip("Жирный (Ctrl+B)");
    toolbarLayout->addWidget(btnBold);

    // Кнопка Italic
    btnItalic = new QPushButton("I", this);
    btnItalic->setMaximumWidth(40);
    btnItalic->setFont(QFont("Segoe UI", 9, QFont::Normal, true));
    btnItalic->setToolTip("Курсив (Ctrl+I)");
    toolbarLayout->addWidget(btnItalic);

    // Кнопка Underline
    btnUnderline = new QPushButton("U", this);
    btnUnderline->setMaximumWidth(40);
    QFont underlineFont = btnUnderline->font();
    underlineFont.setUnderline(true);
    btnUnderline->setFont(underlineFont);
    btnUnderline->setToolTip("Подчёркнутый (Ctrl+U)");
    toolbarLayout->addWidget(btnUnderline);

    // Выпадающий список размера шрифта
    comboFontSize = new QComboBox(this);
    comboFontSize->setMaximumWidth(80);
    comboFontSize->addItems({"8", "9", "10", "11", "12", "14", "16", "18", "20", "24", "28", "32", "36"});
    comboFontSize->setCurrentText("12");
    comboFontSize->setToolTip("Размер шрифта");
    toolbarLayout->addWidget(comboFontSize);

    // Подключение
    connect(comboFontSize, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index){
        if (sourceTextWidget) {
            int size = comboFontSize->currentText().toInt();
            QFont font = sourceTextWidget->currentFont();
            font.setPointSize(size);
            sourceTextWidget->setCurrentFont(font);
            sourceTextWidget->setFocus();
        }
    });
    // Распорка
    toolbarLayout->addStretch();

    // Разделитель между группами кнопок
    toolbarLayout->addSpacing(10);

    // Добавляем toolbar в контейнер
    containerLayout->addLayout(toolbarLayout);

    // Текстовый редактор
    sourceTextWidget = new ScaledTextEdit(this);
    sourceTextWidget->setPlainText("Добавьте файл с помощью кнопки в панели");
    sourceTextWidget->setWordWrapMode(QTextOption::WordWrap);  // Перенос слов
    containerLayout->addWidget(sourceTextWidget);

    // Горячие клавиши
    new QShortcut(QKeySequence("Ctrl+B"), this, [this](){ toggleBold(); });
    new QShortcut(QKeySequence("Ctrl+I"), this, [this](){ toggleItalic(); });
    new QShortcut(QKeySequence("Ctrl+U"), this, [this](){ toggleUnderline(); });

    // Разделитель между группами кнопок
    toolbarLayout->addSpacing(10);

    // Кнопка: Выровнять по левому краю
    QPushButton* btnAlignLeft = new QPushButton("⮜", this);
    btnAlignLeft->setMaximumWidth(40);
    btnAlignLeft->setToolTip("Выровнять по левому краю (Ctrl+L)");
    toolbarLayout->addWidget(btnAlignLeft);

    // Кнопка: Выровнять по центру
    QPushButton* btnAlignCenter = new QPushButton("—", this);
    btnAlignCenter->setMaximumWidth(40);
    btnAlignCenter->setToolTip("Выровнять по центру (Ctrl+E)");
    toolbarLayout->addWidget(btnAlignCenter);

    // Кнопка: Выровнять по правому краю
    QPushButton* btnAlignRight = new QPushButton("⮞", this);
    btnAlignRight->setMaximumWidth(40);
    btnAlignRight->setToolTip("Выровнять по правому краю (Ctrl+R)");
    toolbarLayout->addWidget(btnAlignRight);

    // Кнопка: По ширине
    QPushButton* btnAlignJustify = new QPushButton("☰", this);
    btnAlignJustify->setMaximumWidth(40);
    btnAlignJustify->setToolTip("По ширине (Ctrl+J)");
    toolbarLayout->addWidget(btnAlignJustify);

    // Подключаем кнопки (после создания всех кнопок)
    connect(btnBold, &QPushButton::clicked, this, &MainWindow::toggleBold);
    connect(btnItalic, &QPushButton::clicked, this, &MainWindow::toggleItalic);
    connect(btnUnderline, &QPushButton::clicked, this, &MainWindow::toggleUnderline);

    // Добавь подключение кнопок выравнивания:
    connect(btnAlignLeft, &QPushButton::clicked, this, &MainWindow::alignLeft);
    connect(btnAlignCenter, &QPushButton::clicked, this, &MainWindow::alignCenter);
    connect(btnAlignRight, &QPushButton::clicked, this, &MainWindow::alignRight);
    connect(btnAlignJustify, &QPushButton::clicked, this, &MainWindow::alignJustify);

    // Горячие клавиши выравнивания
    new QShortcut(QKeySequence("Ctrl+L"), this, [this](){ alignLeft(); });
    new QShortcut(QKeySequence("Ctrl+E"), this, [this](){ alignCenter(); });
    new QShortcut(QKeySequence("Ctrl+R"), this, [this](){ alignRight(); });
    new QShortcut(QKeySequence("Ctrl+J"), this, [this](){ alignJustify(); });

    // Разделитель
    toolbarLayout->addSpacing(10);

    // Кнопка: Цвет текста
    btnTextColor = new QPushButton("A", this);
    btnTextColor->setMaximumWidth(40);
    btnTextColor->setToolTip("Цвет текста");

    // Добавляем цветную полоску под буквой (визуальный индикатор)
    QPalette palette = btnTextColor->palette();
    palette.setColor(QPalette::ButtonText, Qt::red);  // Начальный цвет
    btnTextColor->setPalette(palette);

    toolbarLayout->addWidget(btnTextColor);

    // Подключаем кнопку
    connect(btnTextColor, &QPushButton::clicked, this, &MainWindow::changeTextColor);

    // Разделитель
    toolbarLayout->addSpacing(10);

    // Кнопка: Маркированный список
    QPushButton* btnBulletList = new QPushButton("•", this);
    btnBulletList->setMaximumWidth(40);
    btnBulletList->setToolTip("Маркированный список");
    toolbarLayout->addWidget(btnBulletList);

    // Кнопка: Нумерованный список
    QPushButton* btnNumberedList = new QPushButton("1.", this);
    btnNumberedList->setMaximumWidth(40);
    btnNumberedList->setToolTip("Нумерованный список");
    toolbarLayout->addWidget(btnNumberedList);

    // Подключение
    connect(btnBulletList, &QPushButton::clicked, this, &MainWindow::insertBulletList);
    connect(btnNumberedList, &QPushButton::clicked, this, &MainWindow::insertNumberedList);

    // Разделитель
    toolbarLayout->addSpacing(10);

    // Кнопка: Цвет фона (выделение)
    btnHighlightColor = new QPushButton("🖍️", this);
    btnHighlightColor->setMaximumWidth(40);
    btnHighlightColor->setToolTip("Цвет фона (выделение)");
    toolbarLayout->addWidget(btnHighlightColor);

    // Подключение
    connect(btnHighlightColor, &QPushButton::clicked, this, &MainWindow::changeHighlightColor);
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
    splitter->addWidget(textEditorContainer);

    // Создаём контейнер для label + tabWidget
    QWidget* tabContainer = new QWidget(this);
    QVBoxLayout* tabLayout = new QVBoxLayout(tabContainer);
    tabLayout->setContentsMargins(5, 5, 5, 5);
    tabLayout->setSpacing(5);

    // Label для названия группы билетов
    QLabel* groupLabel = new QLabel("Билеты по предмету", this);
    groupLabel->setFont(QFont("Segoe UI", 12, QFont::Bold));
    groupLabel->setAlignment(Qt::AlignCenter);
    groupLabel->setStyleSheet("QLabel { padding: 5px; background: #2b2b2b; color: #ffffff; }");
    tabLayout->addWidget(groupLabel);

    // Сохраняем указатель для последующего изменения текста
    ticketGroupLabel = groupLabel;  // ← Нужно добавить в .h

    groupLabel->setStyleSheet(
        "QLabel { "
        "    padding: 8px; "
        "    background: #3b3b3b; "
        "    color: #ffffff; "
        "    border-radius: 4px; "
        "    font-weight: bold; "
        "}"
    );
    tabLayout->addWidget(ui->tabWidget);
    splitter->addWidget(tabContainer);
    // Виджет с билетами можно убрать разделителем
    splitter->setCollapsible(splitter->indexOf(tabContainer), true);
    // Убираем расстояние между окнами
    splitter->setHandleWidth(0);
}

void MainWindow::toggleBold()
{
    if (sourceTextWidget) {
        // Оборачиваем в блок отмены
        QTextCursor cursor = sourceTextWidget->textCursor();
        cursor.beginEditBlock();
        // Применяем форматирование
        QFont font = sourceTextWidget->currentFont();
        font.setBold(!font.bold());
        sourceTextWidget->setCurrentFont(font);

        cursor.endEditBlock();

        sourceTextWidget->setFocus();
    }
}

void MainWindow::toggleItalic()
{
    if (sourceTextWidget) {
        QTextCursor cursor = sourceTextWidget->textCursor();
        cursor.beginEditBlock();
        QFont font = sourceTextWidget->currentFont();
        font.setItalic(!font.italic());
        sourceTextWidget->setCurrentFont(font);
        cursor.endEditBlock();

        sourceTextWidget->setFocus();
    }
}

void MainWindow::toggleUnderline()
{
    if (sourceTextWidget) {
        QTextCursor cursor = sourceTextWidget->textCursor();
        cursor.beginEditBlock();
        QFont font = sourceTextWidget->currentFont();
        font.setUnderline(!font.underline());
        sourceTextWidget->setCurrentFont(font);
        cursor.endEditBlock();

        sourceTextWidget->setFocus();
    }
}

void MainWindow::alignLeft()
{
    if (sourceTextWidget) {
        sourceTextWidget->setAlignment(Qt::AlignLeft);
        sourceTextWidget->setFocus();
    }
}

void MainWindow::alignCenter()
{
    if (sourceTextWidget) {
        sourceTextWidget->setAlignment(Qt::AlignCenter);
        sourceTextWidget->setFocus();
    }
}

void MainWindow::alignRight()
{
    if (sourceTextWidget) {
        sourceTextWidget->setAlignment(Qt::AlignRight);
        sourceTextWidget->setFocus();
    }
}

void MainWindow::alignJustify()
{
    if (sourceTextWidget) {
        sourceTextWidget->setAlignment(Qt::AlignJustify);
        sourceTextWidget->setFocus();
    }
}

void MainWindow::changeTextColor()
{
    if (!sourceTextWidget) return;

    // Открываем диалог выбора цвета
    QColor initialColor = sourceTextWidget->textColor();
    QColor selectedColor = QColorDialog::getColor(initialColor, this, "Выберите цвет текста");

    // Если пользователь выбрал цвет (не нажал Отмена)
    if (selectedColor.isValid()) {
        // Применяем цвет к выделенному тексту
        QTextCursor cursor = sourceTextWidget->textCursor();
        cursor.beginEditBlock();

        if (cursor.hasSelection()) {
            // Есть выделение — меняем цвет только у него
            QTextCharFormat format;
            format.setForeground(selectedColor);
            cursor.mergeCharFormat(format);
        } else {
            // Нет выделения — меняем цвет по умолчанию для нового текста
            sourceTextWidget->setTextColor(selectedColor);
        }

        // Обновляем цвет кнопки (визуальный индикатор)
        QPalette palette = btnTextColor->palette();
        palette.setColor(QPalette::ButtonText, selectedColor);
        btnTextColor->setPalette(palette);

        sourceTextWidget->setFocus();
    }
}

void MainWindow::insertBulletList()
{
    if (sourceTextWidget) {
        QTextCursor cursor = sourceTextWidget->textCursor();

        // ОДИН блок отмены
        cursor.beginEditBlock();

        QTextListFormat listFormat;
        listFormat.setStyle(QTextListFormat::ListDisc);

        // Если есть выделение — делаем его списком
        if (cursor.hasSelection()) {
            cursor.createList(listFormat);
        } else {
            // Иначе создаём новый список
            cursor.insertList(listFormat);
        }

        cursor.endEditBlock();
        sourceTextWidget->setFocus();
    }
}

void MainWindow::insertNumberedList()
{
    if (sourceTextWidget) {
        QTextCursor cursor = sourceTextWidget->textCursor();

        cursor.beginEditBlock();

        QTextListFormat listFormat;
        listFormat.setStyle(QTextListFormat::ListDecimal);

        if (cursor.hasSelection()) {
            cursor.createList(listFormat);
        } else {
            cursor.insertList(listFormat);
        }

        cursor.endEditBlock();
        sourceTextWidget->setFocus();
    }
}

void MainWindow::changeHighlightColor()
{
    if (!sourceTextWidget) return;

    QTextCursor cursor = sourceTextWidget->textCursor();

    // Если нет выделения — ничего не делаем
    if (!cursor.hasSelection()) {
        QMessageBox::warning(this, "Предупреждение", "Выделите текст для изменения фона");
        return;
    }

    QColor initialColor = cursor.charFormat().background().color();
    QColor selectedColor = QColorDialog::getColor(initialColor, this, "Выберите цвет фона");

    if (selectedColor.isValid()) {
        // Один блок отмены на ВСЮ операцию
        cursor.beginEditBlock();

        QTextCharFormat format;
        format.setBackground(selectedColor);
        cursor.mergeCharFormat(format);

        cursor.endEditBlock();

        sourceTextWidget->setFocus();
    }
}

void MainWindow::setTicketGroupName(const QString& name)
{
    if (ticketGroupLabel) {
        ticketGroupLabel->setText(name);
    }
}
