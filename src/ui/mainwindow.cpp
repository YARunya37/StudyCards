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
    toolbarPanel = new QWidget(this);
    QVBoxLayout* containerLayout = new QVBoxLayout(toolbarPanel);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(5);

    // Toolbar (горизонтальная панель)
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setContentsMargins(5, 5, 5, 0);
    toolbarLayout->setSpacing(5);

    // Кнопка Bold
    QPushButton* btnBold = new QPushButton("B", this);
    btnBold->setMaximumWidth(40);
    btnBold->setFont(QFont("Segoe UI", 9, QFont::Bold));
    btnBold->setToolTip("Жирный (Ctrl+B)");
    toolbarLayout->addWidget(btnBold);

    connect(btnBold, &QPushButton::clicked, this, [this](){
        if (sourceTextWidget) {
            QFont font = sourceTextWidget->currentFont();
            font.setBold(!font.bold());
            sourceTextWidget->setCurrentFont(font);
            sourceTextWidget->setFocus();
        }
    });

    // Кнопка Italic
    QPushButton* btnItalic = new QPushButton("I", this);
    btnItalic->setMaximumWidth(40);
    btnItalic->setFont(QFont("Segoe UI", 9, QFont::Normal, true));
    btnItalic->setToolTip("Курсив (Ctrl+I)");
    toolbarLayout->addWidget(btnItalic);

    connect(btnItalic, &QPushButton::clicked, this, [this](){
        if (sourceTextWidget) {
            QFont font = sourceTextWidget->currentFont();
            font.setItalic(!font.italic());
            sourceTextWidget->setCurrentFont(font);
            sourceTextWidget->setFocus();
        }
    });

    // Кнопка Underline
    QPushButton* btnUnderline = new QPushButton("U", this);
    btnUnderline->setMaximumWidth(40);
    QFont underlineFont = btnUnderline->font();
    underlineFont.setUnderline(true);
    btnUnderline->setFont(underlineFont);
    btnUnderline->setToolTip("Подчёркнутый (Ctrl+U)");
    toolbarLayout->addWidget(btnUnderline);

    connect(btnUnderline, &QPushButton::clicked, this, [this](){
        if (sourceTextWidget) {
            QFont font = sourceTextWidget->currentFont();
            font.setUnderline(!font.underline());
            sourceTextWidget->setCurrentFont(font);
            sourceTextWidget->setFocus();
        }
    });

    // Выпадающий список размера шрифта
    QComboBox* comboFontSize = new QComboBox(this);
    comboFontSize->setMaximumWidth(80);
    comboFontSize->addItems({"8", "9", "10", "11", "12", "14", "16", "18", "20", "24", "28", "32", "36"});
    comboFontSize->setCurrentText("12");
    comboFontSize->setToolTip("Размер шрифта");
    toolbarLayout->addWidget(comboFontSize);

    // Подключение
    connect(comboFontSize, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this, comboFontSize](int index){
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

    // Кнопка: Выровнять по левому краю
    QPushButton* btnAlignLeft = new QPushButton("⮜", this);
    btnAlignLeft->setMaximumWidth(40);
    btnAlignLeft->setToolTip("Выровнять по левому краю (Ctrl+L)");
    toolbarLayout->addWidget(btnAlignLeft);

    connect(btnAlignLeft, &QPushButton::clicked, this, [this](){
        if (sourceTextWidget) {
            sourceTextWidget->setAlignment(Qt::AlignLeft);
            sourceTextWidget->setFocus();
        }
    });

    // Кнопка: Выровнять по центру
    QPushButton* btnAlignCenter = new QPushButton("-", this);
    btnAlignCenter->setMaximumWidth(40);
    btnAlignCenter->setToolTip("Выровнять по центру (Ctrl+E)");
    toolbarLayout->addWidget(btnAlignCenter);

    connect(btnAlignCenter, &QPushButton::clicked, this, [this](){
        if (sourceTextWidget) {
            sourceTextWidget->setAlignment(Qt::AlignCenter);
            sourceTextWidget->setFocus();
        }
    });

    // Кнопка: Выровнять по правому краю
    QPushButton* btnAlignRight = new QPushButton("⮞", this);
    btnAlignRight->setMaximumWidth(40);
    btnAlignRight->setToolTip("Выровнять по правому краю (Ctrl+R)");
    toolbarLayout->addWidget(btnAlignRight);

    connect(btnAlignRight, &QPushButton::clicked, this, [this](){
        if (sourceTextWidget) {
            sourceTextWidget->setAlignment(Qt::AlignRight);
            sourceTextWidget->setFocus();
        }
    });

    // Кнопка: По ширине
    QPushButton* btnAlignJustify = new QPushButton("☰", this);
    btnAlignJustify->setMaximumWidth(40);
    btnAlignJustify->setToolTip("По ширине (Ctrl+J)");
    toolbarLayout->addWidget(btnAlignJustify);

    connect(btnAlignJustify, &QPushButton::clicked, this, [this](){
        if (sourceTextWidget) {
            sourceTextWidget->setAlignment(Qt::AlignJustify);
            sourceTextWidget->setFocus();
        }
    });

    // Разделитель
    toolbarLayout->addSpacing(10);

    // Кнопка: Маркированный список
    QPushButton* btnBulletList = new QPushButton("•", this);
    btnBulletList->setMaximumWidth(40);
    btnBulletList->setToolTip("Маркированный список");
    toolbarLayout->addWidget(btnBulletList);

    connect(btnBulletList, &QPushButton::clicked, this, [this](){
        if (sourceTextWidget) {
            QTextCursor cursor = sourceTextWidget->textCursor();
            cursor.beginEditBlock();
            QTextListFormat listFormat;
            listFormat.setStyle(QTextListFormat::ListDisc);
            if (cursor.hasSelection()) {
                cursor.createList(listFormat);
            } else {
                cursor.insertList(listFormat);
            }
            cursor.endEditBlock();
            sourceTextWidget->setFocus();
        }
    });

    // Кнопка: Нумерованный список
    QPushButton* btnNumberedList = new QPushButton("1.", this);
    btnNumberedList->setMaximumWidth(40);
    btnNumberedList->setToolTip("Нумерованный список");
    toolbarLayout->addWidget(btnNumberedList);

    connect(btnNumberedList, &QPushButton::clicked, this, [this](){
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
    });

    // Разделитель
    toolbarLayout->addSpacing(10);

    // Кнопка: Цвет текста
    QPushButton* btnTextColor = new QPushButton("A", this);
    btnTextColor->setObjectName("btnTextColor");
    btnTextColor->setMaximumWidth(40);
    btnTextColor->setToolTip("Цвет текста");
    toolbarLayout->addWidget(btnTextColor);

    // Добавляем цветную полоску под буквой (визуальный индикатор)
    QPalette palette = btnTextColor->palette();
    palette.setColor(QPalette::ButtonText, Qt::red);  // Начальный цвет
    btnTextColor->setPalette(palette);

    // Подключаем кнопку
    connect(btnTextColor, &QPushButton::clicked, this, &MainWindow::changeTextColor);

    // Разделитель
    toolbarLayout->addSpacing(10);

    // Кнопка: Цвет фона (выделение)
    QPushButton* btnHighlightColor = new QPushButton("🖍️", this);
    btnHighlightColor->setMaximumWidth(40);
    btnHighlightColor->setToolTip("Цвет фона (выделение)");
    toolbarLayout->addWidget(btnHighlightColor);

    // Подключение
    connect(btnHighlightColor, &QPushButton::clicked, this, &MainWindow::changeHighlightColor);

    // Добавляем toolbar в контейнер
    containerLayout->addLayout(toolbarLayout);

    // Текстовый редактор
    sourceTextWidget = new ScaledTextEdit(this);
    sourceTextWidget->setPlainText("Добавьте файл с помощью кнопки в панели");
    sourceTextWidget->setWordWrapMode(QTextOption::WordWrap);  // Перенос слов
    containerLayout->addWidget(sourceTextWidget);

    // Горячие клавиши форматирования
    new QShortcut(QKeySequence("Ctrl+B"), this, [this](){
        if (sourceTextWidget) {
            QFont font = sourceTextWidget->currentFont();
            font.setBold(!font.bold());
            sourceTextWidget->setCurrentFont(font);
            sourceTextWidget->setFocus();
        }
    });

    new QShortcut(QKeySequence("Ctrl+I"), this, [this](){
        if (sourceTextWidget) {
            QFont font = sourceTextWidget->currentFont();
            font.setItalic(!font.italic());
            sourceTextWidget->setCurrentFont(font);
            sourceTextWidget->setFocus();
        }
    });

    new QShortcut(QKeySequence("Ctrl+U"), this, [this](){
        if (sourceTextWidget) {
            QFont font = sourceTextWidget->currentFont();
            font.setUnderline(!font.underline());
            sourceTextWidget->setCurrentFont(font);
            sourceTextWidget->setFocus();
        }
    });

    // Горячие клавиши выравнивания
    new QShortcut(QKeySequence("Ctrl+L"), this, [this](){
        if (sourceTextWidget) {
            sourceTextWidget->setAlignment(Qt::AlignLeft);
            sourceTextWidget->setFocus();
        }
    });

    new QShortcut(QKeySequence("Ctrl+E"), this, [this](){
        if (sourceTextWidget) {
            sourceTextWidget->setAlignment(Qt::AlignCenter);
            sourceTextWidget->setFocus();
        }
    });

    new QShortcut(QKeySequence("Ctrl+R"), this, [this](){
        if (sourceTextWidget) {
            sourceTextWidget->setAlignment(Qt::AlignRight);
            sourceTextWidget->setFocus();
        }
    });

    new QShortcut(QKeySequence("Ctrl+J"), this, [this](){
        if (sourceTextWidget) {
            sourceTextWidget->setAlignment(Qt::AlignJustify);
            sourceTextWidget->setFocus();
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
    splitter->addWidget(toolbarPanel);

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

        cursor.endEditBlock();

        // Обновляем цвет кнопки (визуальный индикатор)
        QPushButton* btn = this->findChild<QPushButton*>("btnTextColor");
        if (btn) {
            QPalette palette = btn->palette();
            palette.setColor(QPalette::ButtonText, selectedColor);
            btn->setPalette(palette);
        }

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
