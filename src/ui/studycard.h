#ifndef STUDYCARD_H
#define STUDYCARD_H

#include <QWidget>
#include <QTextEdit>
#include "filemanager.h"

class StudyCardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StudyCardWidget(QWidget *parent, const QString& path_to_card, const QString& card_name);

    const QString Name() const;
signals:

private:
    // Имя билета
    QString name;

    QTextEdit* header;
    QTextEdit* body;
    // Для управления файлами связанными с текстом билетов
    FileManager fmn;
};

#endif // STUDYCARD_H
