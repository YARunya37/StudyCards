#include "textinputdialog.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

TextInputDialog::TextInputDialog(QWidget *parent, const QString& title, const QString& label)
    : QDialog{parent}, title{title}, label_text{label}
{
    setupUI();
}

QString TextInputDialog::getText() const
{
    return m_lineEdit->text();
}

void TextInputDialog::setupUI()
{
    setWindowTitle(title);

    // Создание виджетов
    QLabel *label = new QLabel(label_text, this);
    m_lineEdit = new QLineEdit(this);
    QPushButton* m_okButton = new QPushButton("OK", this);
    QPushButton* m_cancelButton = new QPushButton("Отмена", this);

    // Компоновка
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addWidget(m_lineEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_okButton);

    mainLayout->addLayout(buttonLayout);

    // Подключение сигналов
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Размер окна
    setMinimumWidth(300);
}
