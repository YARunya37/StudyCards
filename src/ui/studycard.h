#ifndef STUDYCARD_H
#define STUDYCARD_H

#include <QWidget>
#include <QTextEdit>
#include "filemanager.h"
#include "namedfileitem.h"
#include <QString>
class StudyCardWidget : public QWidget, public NamedFileItem
{
    Q_OBJECT
public:
    explicit StudyCardWidget(QWidget *parent, const QString& path_to_card, const QString& card_name);
    ~StudyCardWidget();

    // Методы для получения содержимого header и body. Возвращает html
    QString GetHeaderContent();
    // Методы для получения содержимого header и body. Возвращает html
    QString GetBodyContent();
signals:
    void header_changed(const QString& new_header);
private slots:
    void save_to_files();

public slots:
    void UpdateFilePath(const QString& new_name);

protected:
    // Переопределён, чтобы при закрытии виджета всегда сохранялись изменения
    void hideEvent(QHideEvent *event) override;

private:
    QTextEdit* header;
    QTextEdit* body;
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
