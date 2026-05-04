#include "studycard.h"
#include <QVBoxLayout>
#include <QFont>
#include <QCoreApplication>
#include <QTimer>
StudyCardWidget::StudyCardWidget(QWidget *parent, const QString& local_path_to_card, const QString& card_name)
    : QWidget{parent},
    fmn(this, local_path_to_card),
    NamedFileItem{card_name}
{
    SetUpUI();
    // Если есть файлы, то восстанавливаем текст
    if(!fmn.get_existing_files().isEmpty()){
        RestoreText();
    }
    else{
        fmn.create_files(QStringList() << "header" << "body");
        save_to_files();
    }
    // Для сохранения изменений

    // Используем таймер, который по истечении вызовет сохранение текущего состояния виджетов
    QTimer *saveTimer = new QTimer(this);
    saveTimer->setSingleShot(true);
    saveTimer->setInterval(1000); // 2 секунды после последнего изменения

    connect(header, &QTextEdit::textChanged, [saveTimer]() {
        saveTimer->start(); // перезапускаем таймер
    });
    connect(body, &QTextEdit::textChanged, [saveTimer]() {
        saveTimer->start(); // перезапускаем таймер
    });
    connect(saveTimer, &QTimer::timeout, this, &StudyCardWidget::save_to_files);

}


void StudyCardWidget::save_to_files()
{
    QStringList files = fmn.get_existing_files();
    if(files.length() < 2){
        fmn.create_files(QStringList() << "header" << "body");
    }
    else{
        foreach(auto file, files){
            if(file == "header"){
                fmn.write_to_file(file, header->toHtml());
            }
            else{
                fmn.write_to_file(file, body->toHtml());
            }
        }
    }
    emit header_changed(header->toPlainText());
}

bool StudyCardWidget::RestoreText()
{
    QStringList files = fmn.get_existing_files();
    // Если файлы в директории есть, то восстанавливаем
    if(!files.isEmpty()){
        foreach (auto file, files) {
            if(file == "header"){
                header->setHtml(fmn.get_file_content(file));
                DrawHeader();
            }else{
                body->setHtml(fmn.get_file_content(file));
            }
        }

        return true;
    }
    else{
        fmn.create_files(QStringList() << "header" << "body");
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
    this->header = new QTextEdit(this);
    this->body = new QTextEdit(this);

    layout->addWidget(header);
    layout->addWidget(body);

    // Настройка header

    QFont header_font = header->font();

    header->setPlainText(Name());

    header_font.setPointSize(20);
    header_font.setBold(true);

    header->setFont(header_font);

    header->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    header->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    DrawHeader();

    // При изменении заголовка его размер будет автоматически подгоняться
    connect(header->document(), &QTextDocument::contentsChanged, [this]() {
        int h = header->document()->size().height() + 5;
        header->setFixedHeight(qMin(h, 100)); // Максимум 100px
    }
    );

    // Настройка body
    QFont body_font = body->font();

    body->setPlainText("Текст Вашего билета");

    body_font.setPointSize(16);

    body->setFont(body_font);

    body->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
}

void StudyCardWidget::DrawHeader()
{
    // Используем QFontMetrics для вычисления высоты
    QFontMetrics fm(header->font());
    int lineHeight = fm.lineSpacing();
    int lines = header->document()->lineCount();
    int newHeight = qBound(50, lineHeight * lines + 10, 100);
    header->setFixedHeight(newHeight);
}
