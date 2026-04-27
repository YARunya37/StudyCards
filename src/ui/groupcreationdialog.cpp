#include "groupcreationdialog.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

GroupCreationDialog::GroupCreationDialog(QWidget *parent)
    : QDialog{parent}
{
    setupUI();
}

QString GroupCreationDialog::getText() const
{
    return m_lineEdit->text();
}

void GroupCreationDialog::setupUI()
{
    setWindowTitle("Введите название группы");

    // Создание виджетов
    QLabel *label = new QLabel("Название:", this);
    m_lineEdit = new QLineEdit(this);
    m_okButton = new QPushButton("OK", this);
    m_cancelButton = new QPushButton("Отмена", this);

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
