#ifndef TEXTINPUTDIALOG_H
#define TEXTINPUTDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QLineEdit>

class TextInputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TextInputDialog(QWidget *parent, const QString& title, const QString& label);
    QString getText() const;

private:
    void setupUI();
    QString title;
    QString label_text;

    QLineEdit *m_lineEdit;
};

#endif // TEXTINPUTDIALOG_H
