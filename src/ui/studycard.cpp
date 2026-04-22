#include "studycard.h"
#include <QVBoxLayout>
#include <QFont>
StudyCard::StudyCard(QWidget *parent)
    : QWidget{parent}
{
    // Создаём layout для организации
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Добавляем в layout поля для названия и текста билета
    header = new QTextEdit(this);
    body = new QTextEdit(this);

    layout->addWidget(header);
    layout->addWidget(body);

    // Настройка header
    header->setPlainText("Название билета");

    QFont header_font = header->font();

    header_font.setPointSize(20);
    header_font.setBold(true);

    header->setFont(header_font);

    header->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    header->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Зафиксированная высота, вычисленная на практике для 20 размера шрифта
    header->setFixedHeight(50);

    // При изменении заголовка его размер будет автоматически подгоняться
    connect(header->document(), &QTextDocument::contentsChanged, [this]() {
            int h = header->document()->size().height() + 5;
            header->setFixedHeight(qMin(h, 100)); // Максимум 100px
        }
    );

    // Настройка body
    QFont body_font = body->font();

    body_font.setPointSize(16);

    body->setFont(body_font);

    body->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    body->setPlainText("Текст вашего билета");
}
