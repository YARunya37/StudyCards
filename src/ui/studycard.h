#ifndef STUDYCARD_H
#define STUDYCARD_H

#include "filemanager.h"
#include "namedfileitem.h"
#include "richtextedit.h"

#include <QWidget>
#include <QTextEdit>
#include <QString>
class StudyCardWidget : public QWidget, public NamedFileItem
{
    Q_OBJECT
public:
    explicit StudyCardWidget(QWidget *parent, const QString& path_to_card, const QString& card_name);
    ~StudyCardWidget();

    // Методы для получения содержимого header и body. Возвращает html
    QString GetHeaderContent();
    QString GetBodyContent();

    void SetQuestionText(const QString& text);
    QString GetQuestionText() const;
    QString GetQuestionTextFromFile() const;
signals:
    void header_changed(const QString& new_header);

private slots:
    void save_to_files();

protected:
    // Переопределён, чтобы при закрытии виджета всегда сохранялись изменения
    void hideEvent(QHideEvent *event) override;

private:
    RichTextEdit* header;
    RichTextEdit* body;
    // Для управления файлами связанными с текстом билетов
    FileManager* fmn;
    // Метод для восстановления текста билетов. true если текст был восстановлен
    bool RestoreText();
    // Метод, который собирает интерфейс виджета
    void SetUpUI();
    // Метод для выставления высоты header
    void DrawHeader();

    // Путь к билету
    QString localPathToGroup;
};

#endif // STUDYCARD_H
