#ifndef TEXTFORMATTINGTOOLBAR_H
#define TEXTFORMATTINGTOOLBAR_H

#pragma once

#include <QWidget>
#include <QTextEdit>

class QPushButton;
class QComboBox;

class TextFormattingToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit TextFormattingToolbar(QWidget* parent = nullptr);

    void setActiveEditor(QTextEdit* editor);
    QTextEdit* activeEditor() const { return m_currentEditor; }

private slots:
    void toggleBold();
    void toggleItalic();
    void toggleUnderline();
    void alignLeft();
    void alignCenter();
    void alignRight();
    void alignJustify();
    void changeTextColor();
    void changeHighlightColor();
    void insertBulletList();
    void insertNumberedList();
    void changeFontSize(int size);

private:
    QTextEdit* m_currentEditor;  // Текущий редактор

    QPushButton* btnTextColor;
    QPushButton* btnHighlightColor;
    QComboBox* comboFontSize;

    void createToolbar();
};

#endif // TEXTFORMATTINGTOOLBAR_H
