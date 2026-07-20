#include "studycard.h"
#include "richtextedit.h"
#include <QVBoxLayout>
#include <QFont>
#include <QCoreApplication>
#include <QTimer>
#include <QFileInfo>

StudyCardWidget::StudyCardWidget(QWidget *parent, const QString& local_path_to_group, const QString& card_name)
    : QWidget{parent},
    fmn{new FileManager(this, local_path_to_group + "/" + card_name)},
    NamedFileItem{card_name}
{
    localPathToGroup = local_path_to_group;
    SetUpUI();
    // Если есть файлы, то восстанавливаем текст
    if(!fmn->get_existing_files().isEmpty()){
        RestoreText();
    }
    else{
        fmn->create_files(QStringList() << "header" << "body");
        save_to_files();
    }

    // Используем таймер, который по истечении вызовет сохранение текущего состояния виджетов
    QTimer *saveTimer = new QTimer(this);
    saveTimer->setSingleShot(true);
    saveTimer->setInterval(100); // десятую секунды после последнего изменения

    connect(header, &QTextEdit::textChanged, [saveTimer]() {
        saveTimer->start(); // перезапускаем таймер
    });
    connect(body, &QTextEdit::textChanged, [saveTimer]() {
        saveTimer->start(); // перезапускаем таймер
    });
    connect(saveTimer, &QTimer::timeout, this, &StudyCardWidget::save_to_files);
}

StudyCardWidget::~StudyCardWidget()
{
    delete fmn;
    fmn = nullptr;
}

QString StudyCardWidget::GetHeaderContent()
{
    return header->toHtml();
}

QString StudyCardWidget::GetBodyContent()
{
    return body->toHtml();
}

void StudyCardWidget::SetQuestionText(const QString& text)
{
    header->setHtml(text);
    save_to_files();
}

QString StudyCardWidget::GetQuestionText() const
{
    return header->toPlainText();
}

QString StudyCardWidget::GetQuestionTextFromFile() const
{
    // Читаем напрямую из header.html
    QString content = fmn->get_file_content("header");
    // Конвертируем HTML в plain text
    QTextDocument doc;
    doc.setHtml(content);
    return doc.toPlainText();
}

void StudyCardWidget::SetName(const QString &new_name)
{
    NamedFileItem::SetName(new_name);
    delete fmn;
    fmn = new FileManager(this, localPathToGroup + "/" + new_name);
}

void StudyCardWidget::save_to_files()
{
    QStringList files = fmn->get_existing_files();
    if(files.length() < 2){
        fmn->create_files(QStringList() << "header" << "body");
    }

    fmn->write_to_file("header", header->toHtml());
    fmn->write_to_file("body", body->toHtml());

    emit header_changed(header->toPlainText());
}

void StudyCardWidget::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    // Сохраняем изменения
    save_to_files();

}

bool StudyCardWidget::RestoreText()
{
    QStringList files = fmn->get_existing_files();
    // Если файлы в директории есть, то восстанавливаем
    if(!files.isEmpty()){
        foreach (auto file, files) {
            if(file == "header"){
                header->setHtml(fmn->get_file_content(file));
                DrawHeader();
            }else{
                body->setHtml(fmn->get_file_content(file));
            }
        }

        return true;
    }
    else{
        fmn->create_files(QStringList() << "header" << "body");
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
    this->header = new RichTextEdit(this);
    this->body = new RichTextEdit(this);

    layout->addWidget(header);
    layout->addWidget(body);

    // Настройка header
    QFont header_font = header->font();
    header->setPlainText(name);
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
