#ifndef GROUPCREATIONDIALOG_H
#define GROUPCREATIONDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QLineEdit>

class GroupCreationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GroupCreationDialog(QWidget *parent = nullptr);
    QString getText() const;

private:
    void setupUI();

    QLineEdit *m_lineEdit;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
};

#endif // GROUPCREATIONDIALOG_H
