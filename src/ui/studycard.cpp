#include "studycard.h"
#include <QVBoxLayout>
#include <QFont>
#include <QCoreApplication>
StudyCardWidget::StudyCardWidget(QWidget *parent, const QString& local_path_to_card, const QString& card_name)
    : QWidget{parent},
    fmn(this, local_path_to_card),
    name{card_name}
{
    // Если мы не восстановили билет из файла, то инициализируем текст в нём значениями по умолчанию
    if(!RestoreText()){
        header->setPlainText(name);
        body->setPlainText("Текст Вашего билета");
    }

    SetUpUI();
}

bool StudyCardWidget::RestoreText()
{
    QStringList files = fmn.get_existing_files();
    // Если файлы в директории есть, то восстанавливаем
    if(!files.isEmpty()){
        foreach (auto file, files) {
            if(file == "header"){
                header->setHtml(fmn.get_file_content(file));
            }else{
                body->setHtml(fmn.get_file_content(file));
            }
        }

        return true;
    }
    else{
        return false;
    }
}

void StudyCardWidget::SetUpUI()
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
}
