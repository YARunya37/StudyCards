#include "textformattingtoolbar.h"

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
{
    createToolbar();
}

void TextFormattingToolbar::setActiveEditor(QTextEdit* editor)
{
    m_currentEditor = editor;
}

void TextFormattingToolbar::createToolbar()
{
    // Вертикальный layout для всего виджета
    QVBoxLayout* containerLayout = new QVBoxLayout(this);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(5);

    // Горизонтальный toolbar
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setContentsMargins(5, 5, 5, 0);
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
    comboFontSize->setMaximumWidth(80);
    comboFontSize->addItems({"8", "9", "10", "11", "12", "14", "16", "18", "20", "24", "28", "32", "36"});
    comboFontSize->setCurrentText("12");
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
}

// Реализация слотов
void TextFormattingToolbar::toggleBold()
{
    if (!m_currentEditor) return;

    QFont font = m_currentEditor->currentFont();
    font.setBold(!font.bold());
    m_currentEditor->setCurrentFont(font);
    m_currentEditor->setFocus();
}

void TextFormattingToolbar::toggleItalic()
{
    if (!m_currentEditor) return;

    QFont font = m_currentEditor->currentFont();
    font.setItalic(!font.italic());
    m_currentEditor->setCurrentFont(font);
    m_currentEditor->setFocus();
}

void TextFormattingToolbar::toggleUnderline()
{
    if (!m_currentEditor) return;

    QFont font = m_currentEditor->currentFont();
    font.setUnderline(!font.underline());
    m_currentEditor->setCurrentFont(font);
    m_currentEditor->setFocus();
}

void TextFormattingToolbar::alignLeft()
{
    if (!m_currentEditor) return;
    m_currentEditor->setAlignment(Qt::AlignLeft);
    m_currentEditor->setFocus();
}

void TextFormattingToolbar::alignCenter()
{
    if (!m_currentEditor) return;
    m_currentEditor->setAlignment(Qt::AlignCenter);
    m_currentEditor->setFocus();
}

void TextFormattingToolbar::alignRight()
{
    if (!m_currentEditor) return;
    m_currentEditor->setAlignment(Qt::AlignRight);
    m_currentEditor->setFocus();
}

void TextFormattingToolbar::alignJustify()
{
    if (!m_currentEditor) return;
    m_currentEditor->setAlignment(Qt::AlignJustify);
    m_currentEditor->setFocus();
}

void TextFormattingToolbar::changeTextColor()
{
    if (!m_currentEditor) return;

    QColor initialColor = m_currentEditor->textColor();
    QColor selectedColor = QColorDialog::getColor(initialColor, this, "Выберите цвет текста");

    if (selectedColor.isValid()) {
        QTextCursor cursor = m_currentEditor->textCursor();
        cursor.beginEditBlock();

        if (cursor.hasSelection()) {
            QTextCharFormat format;
            format.setForeground(selectedColor);
            cursor.mergeCharFormat(format);
        } else {
            m_currentEditor->setTextColor(selectedColor);
        }

        cursor.endEditBlock();

        // Обновляем цвет кнопки
        QPalette palette = btnTextColor->palette();
        palette.setColor(QPalette::ButtonText, selectedColor);
        btnTextColor->setPalette(palette);

        m_currentEditor->setFocus();
    }
}

void TextFormattingToolbar::changeHighlightColor()
{
    if (!m_currentEditor) return;

    QTextCursor cursor = m_currentEditor->textCursor();

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

        m_currentEditor->setFocus();
    }
}

void TextFormattingToolbar::insertBulletList()
{
    if (!m_currentEditor) return;

    QTextCursor cursor = m_currentEditor->textCursor();
    cursor.beginEditBlock();

    QTextListFormat listFormat;
    listFormat.setStyle(QTextListFormat::ListDisc);

    if (cursor.hasSelection()) {
        cursor.createList(listFormat);
    } else {
        cursor.insertList(listFormat);
    }

    cursor.endEditBlock();
    m_currentEditor->setFocus();
}

void TextFormattingToolbar::insertNumberedList()
{
    if (!m_currentEditor) return;

    QTextCursor cursor = m_currentEditor->textCursor();
    cursor.beginEditBlock();

    QTextListFormat listFormat;
    listFormat.setStyle(QTextListFormat::ListDecimal);

    if (cursor.hasSelection()) {
        cursor.createList(listFormat);
    } else {
        cursor.insertList(listFormat);
    }

    cursor.endEditBlock();
    m_currentEditor->setFocus();
}

void TextFormattingToolbar::changeFontSize(int size)
{
    if (!m_currentEditor) return;

    int fontSize = comboFontSize->currentText().toInt();
    QFont font = m_currentEditor->currentFont();
    font.setPointSize(fontSize);
    m_currentEditor->setCurrentFont(font);
    m_currentEditor->setFocus();
}
