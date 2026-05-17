#include "textformattingtoolbar.h"

#include <QApplication>
#include <QFocusEvent>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFontDialog>
#include <QColorDialog>
#include <QTextCursor>
#include <QTextListFormat>
#include <QShortcut>
#include <QKeySequence>

TextFormattingToolbar::TextFormattingToolbar(QWidget* parent)
    : QWidget(parent)
    , m_currentEditor(nullptr)
    , m_lastActiveEditor(nullptr)
{
    createToolbar();

    // Отслеживаем смену фокуса:
    connect(qApp, &QApplication::focusChanged, this, [this](QWidget* oldWidget, QWidget* now) {
        Q_UNUSED(oldWidget);
        if (QTextEdit* editor = qobject_cast<QTextEdit*>(now)) {
            m_lastActiveEditor = editor;  // Запоминаем последний активный редактор
        }
    });
}

void TextFormattingToolbar::setActiveEditor(QTextEdit* editor)
{
    m_currentEditor = editor;
    m_lastActiveEditor = editor;
}

void TextFormattingToolbar::createToolbar()
{
    setFixedHeight(36);
    // Вертикальный layout для всего виджета
    QVBoxLayout* containerLayout = new QVBoxLayout(this);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    // Горизонтальный toolbar
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setContentsMargins(5, 5, 5, 5);
    toolbarLayout->setSpacing(5);

    // Кнопка Bold
    QPushButton* btnBold = new QPushButton("B", this);
    btnBold->setMaximumWidth(40);
    btnBold->setFont(QFont("Segoe UI", 9, QFont::Bold));
    btnBold->setToolTip("Жирный (Ctrl+B)");
    toolbarLayout->addWidget(btnBold);
    connect(btnBold, &QPushButton::clicked, this, &TextFormattingToolbar::toggleBold);

    // Кнопка Italic
    QPushButton* btnItalic = new QPushButton("I", this);
    btnItalic->setMaximumWidth(40);
    btnItalic->setFont(QFont("Segoe UI", 9, QFont::Normal, true));
    btnItalic->setToolTip("Курсив (Ctrl+I)");
    toolbarLayout->addWidget(btnItalic);
    connect(btnItalic, &QPushButton::clicked, this, &TextFormattingToolbar::toggleItalic);

    // Кнопка Underline
    QPushButton* btnUnderline = new QPushButton("U", this);
    btnUnderline->setMaximumWidth(40);
    QFont underlineFont = btnUnderline->font();
    underlineFont.setUnderline(true);
    btnUnderline->setFont(underlineFont);
    btnUnderline->setToolTip("Подчёркнутый (Ctrl+U)");
    toolbarLayout->addWidget(btnUnderline);
    connect(btnUnderline, &QPushButton::clicked, this, &TextFormattingToolbar::toggleUnderline);

    // Combobox размера шрифта
    comboFontSize = new QComboBox(this);
    comboFontSize->setFixedHeight(30);
    comboFontSize->addItems({"8", "9", "10", "11", "12", "14", "16", "18", "20", "24", "28", "32", "36"});
    comboFontSize->setCurrentText("14");
    comboFontSize->setToolTip("Размер шрифта");
    toolbarLayout->addWidget(comboFontSize);
    connect(comboFontSize, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TextFormattingToolbar::changeFontSize);

    // Распорка
    toolbarLayout->addStretch();
    toolbarLayout->addSpacing(10);

    // Кнопки выравнивания
    QPushButton* btnAlignLeft = new QPushButton("⮜", this);
    btnAlignLeft->setMaximumWidth(40);
    btnAlignLeft->setToolTip("По левому краю (Ctrl+L)");
    toolbarLayout->addWidget(btnAlignLeft);
    connect(btnAlignLeft, &QPushButton::clicked, this, &TextFormattingToolbar::alignLeft);

    QPushButton* btnAlignCenter = new QPushButton("-", this);
    btnAlignCenter->setMaximumWidth(40);
    btnAlignCenter->setToolTip("По центру (Ctrl+E)");
    toolbarLayout->addWidget(btnAlignCenter);
    connect(btnAlignCenter, &QPushButton::clicked, this, &TextFormattingToolbar::alignCenter);

    QPushButton* btnAlignRight = new QPushButton("⮞", this);
    btnAlignRight->setMaximumWidth(40);
    btnAlignRight->setToolTip("По правому краю (Ctrl+R)");
    toolbarLayout->addWidget(btnAlignRight);
    connect(btnAlignRight, &QPushButton::clicked, this, &TextFormattingToolbar::alignRight);

    QPushButton* btnAlignJustify = new QPushButton("☰", this);
    btnAlignJustify->setMaximumWidth(40);
    btnAlignJustify->setToolTip("По ширине (Ctrl+J)");
    toolbarLayout->addWidget(btnAlignJustify);
    connect(btnAlignJustify, &QPushButton::clicked, this, &TextFormattingToolbar::alignJustify);

    toolbarLayout->addSpacing(10);

    // Списки
    QPushButton* btnBulletList = new QPushButton("•", this);
    btnBulletList->setMaximumWidth(40);
    btnBulletList->setToolTip("Маркированный список");
    toolbarLayout->addWidget(btnBulletList);
    connect(btnBulletList, &QPushButton::clicked, this, &TextFormattingToolbar::insertBulletList);

    QPushButton* btnNumberedList = new QPushButton("1.", this);
    btnNumberedList->setMaximumWidth(40);
    btnNumberedList->setToolTip("Нумерованный список");
    toolbarLayout->addWidget(btnNumberedList);
    connect(btnNumberedList, &QPushButton::clicked, this, &TextFormattingToolbar::insertNumberedList);

    toolbarLayout->addSpacing(10);

    // Цвет текста
    btnTextColor = new QPushButton("A", this);
    btnTextColor->setMaximumWidth(40);
    btnTextColor->setToolTip("Цвет текста");
    toolbarLayout->addWidget(btnTextColor);
    connect(btnTextColor, &QPushButton::clicked, this, &TextFormattingToolbar::changeTextColor);

    // Цвет фона
    btnHighlightColor = new QPushButton("🖍️", this);
    btnHighlightColor->setMaximumWidth(40);
    btnHighlightColor->setToolTip("Цвет фона (выделение)");
    toolbarLayout->addWidget(btnHighlightColor);
    connect(btnHighlightColor, &QPushButton::clicked, this, &TextFormattingToolbar::changeHighlightColor);

    // Добавляем toolbar в контейнер
    containerLayout->addLayout(toolbarLayout);

    // Горячие клавиши
    new QShortcut(QKeySequence("Ctrl+B"), this, this, &TextFormattingToolbar::toggleBold);
    new QShortcut(QKeySequence("Ctrl+I"), this, this, &TextFormattingToolbar::toggleItalic);
    new QShortcut(QKeySequence("Ctrl+U"), this, this, &TextFormattingToolbar::toggleUnderline);
    new QShortcut(QKeySequence("Ctrl+L"), this, this, &TextFormattingToolbar::alignLeft);
    new QShortcut(QKeySequence("Ctrl+E"), this, this, &TextFormattingToolbar::alignCenter);
    new QShortcut(QKeySequence("Ctrl+R"), this, this, &TextFormattingToolbar::alignRight);
    new QShortcut(QKeySequence("Ctrl+J"), this, this, &TextFormattingToolbar::alignJustify);

    // Стили
    QString containerStyle = R"(
        TextFormattingToolbar {
            background-color: #d0d0d0;
            border-radius: 4px;
        }
    )";
    this->setStyleSheet(containerStyle);

    // Стиль для кнопок форматирования
    QString buttonStyle = R"(
    QPushButton {
        background-color: #ffffff;
        border: 1px solid #d0d0d0;
        border-radius: 3px;
        padding: 0px;
        color: #2c3e50;
        font-size: 10pt;
    }

    QPushButton:hover {
        background-color: #e8f4f8;
        border: 1px solid #0078d4;
    }

    QPushButton:pressed {
        background-color: #d0e8f4;
        border: 1px solid #0078d4;
    }

    QPushButton:checked {
        background-color: #0078d4;
        border: 1px solid #0078d4;
        color: white;
    }
)";

    // Применяем стиль ко всем кнопкам
    btnBold->setStyleSheet(buttonStyle);
    btnItalic->setStyleSheet(buttonStyle);
    btnUnderline->setStyleSheet(buttonStyle);
    btnAlignLeft->setStyleSheet(buttonStyle);
    btnAlignCenter->setStyleSheet(buttonStyle);
    btnAlignRight->setStyleSheet(buttonStyle);
    btnAlignJustify->setStyleSheet(buttonStyle);
    btnBulletList->setStyleSheet(buttonStyle);
    btnNumberedList->setStyleSheet(buttonStyle);
    btnTextColor->setStyleSheet(buttonStyle);
    btnHighlightColor->setStyleSheet(buttonStyle);
    QString comboBoxStyle = R"(
    QComboBox {
        background-color: #ffffff;
        border: 1px solid #d0d0d0;
        border-radius: 3px;
        padding: 0px 8px;
        color: #2c3e50;
    }

    QComboBox:hover {
        border: 1px solid #0078d4;
        background-color: #f8f8f8;
    }

    QComboBox::drop-down {
        border: none;
        width: 18px;
    }

    QComboBox::down-arrow {
        width: 0;
        height: 0;
        border-left: 4px solid transparent;
        border-right: 4px solid transparent;
        border-top: 4px solid #2c3e50;
    }
)";

    comboFontSize->setStyleSheet(comboBoxStyle);

    const int BUTTON_SIZE = 30; // Ширина = Высота

    btnBold->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    btnItalic->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    btnUnderline->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    btnAlignLeft->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    btnAlignCenter->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    btnAlignRight->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    btnAlignJustify->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    btnBulletList->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    btnNumberedList->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    btnTextColor->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    btnHighlightColor->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
}

QTextEdit* TextFormattingToolbar::getCurrentEditor() const
{
    // Сначала ищем в фокусе
    QWidget* focused = QApplication::focusWidget();
    if (QTextEdit* editor = qobject_cast<QTextEdit*>(focused)) {
        return editor;  // Вернёт текущий виджет в фокусе
    }

    // Если фокус не на QTextEdit — используем последний активный
    if (m_lastActiveEditor) {
        return m_lastActiveEditor;  //Используем последний!
    }

    // Если совсем ничего — используем установленный
    if (m_currentEditor) {
        return m_currentEditor;
    }
    return nullptr;
}

// Реализация слотов
void TextFormattingToolbar::toggleBold()
{
    QTextEdit* editor = getCurrentEditor();
    if (!editor) return;

    QFont font = editor->currentFont();
    font.setBold(!font.bold());
    editor->setCurrentFont(font);
    editor->setFocus();
}

void TextFormattingToolbar::toggleItalic()
{
    QTextEdit* editor = getCurrentEditor();
    if (!editor) return;

    QFont font = editor->currentFont();
    font.setItalic(!font.italic());
    editor->setCurrentFont(font);
    editor->setFocus();
}

void TextFormattingToolbar::toggleUnderline()
{
    QTextEdit* editor = getCurrentEditor();
    if (!editor) return;

    QFont font = editor->currentFont();
    font.setUnderline(!font.underline());
    editor->setCurrentFont(font);
    editor->setFocus();
}

void TextFormattingToolbar::alignLeft()
{
    QTextEdit* editor = getCurrentEditor();
    if (!editor) return;
    editor->setAlignment(Qt::AlignLeft);
    editor->setFocus();
}

void TextFormattingToolbar::alignCenter()
{
    QTextEdit* editor = getCurrentEditor();
    if (!editor) return;
    editor->setAlignment(Qt::AlignCenter);
    editor->setFocus();
}

void TextFormattingToolbar::alignRight()
{
    QTextEdit* editor = getCurrentEditor();
    if (!editor) return;
    editor->setAlignment(Qt::AlignRight);
    editor->setFocus();
}

void TextFormattingToolbar::alignJustify()
{
    QTextEdit* editor = getCurrentEditor();
    if (!editor) return;
    editor->setAlignment(Qt::AlignJustify);
    editor->setFocus();
}

void TextFormattingToolbar::changeTextColor()
{
    QTextEdit* editor = getCurrentEditor();
    if (!editor) return;

    QColor initialColor = editor->textColor();
    QColor selectedColor = QColorDialog::getColor(initialColor, this, "Выберите цвет текста");

    if (selectedColor.isValid()) {
        QTextCursor cursor = editor->textCursor();
        cursor.beginEditBlock();

        if (cursor.hasSelection()) {
            QTextCharFormat format;
            format.setForeground(selectedColor);
            cursor.mergeCharFormat(format);
        } else {
            editor->setTextColor(selectedColor);
        }

        cursor.endEditBlock();

        // Обновляем цвет кнопки
        QPalette palette = btnTextColor->palette();
        palette.setColor(QPalette::ButtonText, selectedColor);
        btnTextColor->setPalette(palette);

        editor->setFocus();
    }
}

void TextFormattingToolbar::changeHighlightColor()
{
    QTextEdit* editor = getCurrentEditor();
    if (!editor) return;

    QTextCursor cursor = editor->textCursor();

    if (!cursor.hasSelection()) {
        return;  // Нет выделения
    }

    QColor initialColor = cursor.charFormat().background().color();
    QColor selectedColor = QColorDialog::getColor(initialColor, this, "Выберите цвет фона");

    if (selectedColor.isValid()) {
        cursor.beginEditBlock();

        QTextCharFormat format;
        format.setBackground(selectedColor);
        cursor.mergeCharFormat(format);

        cursor.endEditBlock();

        editor->setFocus();
    }
}

void TextFormattingToolbar::insertBulletList()
{
    QTextEdit* editor = getCurrentEditor();
    if (!editor) return;

    QTextCursor cursor = editor->textCursor();
    cursor.beginEditBlock();

    QTextListFormat listFormat;
    listFormat.setStyle(QTextListFormat::ListDisc);

    if (cursor.hasSelection()) {
        cursor.createList(listFormat);
    } else {
        cursor.insertList(listFormat);
    }

    cursor.endEditBlock();
    editor->setFocus();
}

void TextFormattingToolbar::insertNumberedList()
{
    QTextEdit* editor = getCurrentEditor();
    if (!editor) return;

    QTextCursor cursor = editor->textCursor();
    cursor.beginEditBlock();

    QTextListFormat listFormat;
    listFormat.setStyle(QTextListFormat::ListDecimal);

    if (cursor.hasSelection()) {
        cursor.createList(listFormat);
    } else {
        cursor.insertList(listFormat);
    }

    cursor.endEditBlock();
    editor->setFocus();
}

void TextFormattingToolbar::changeFontSize(int size)
{
    QTextEdit* editor = getCurrentEditor();
    if (!editor) return;

    int fontSize = comboFontSize->currentText().toInt();
    QFont font = editor->currentFont();
    font.setPointSize(fontSize);
    editor->setCurrentFont(font);
    editor->setFocus();
}
