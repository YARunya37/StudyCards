#ifndef STUDYCARD_H
#define STUDYCARD_H

#include <QWidget>
#include <QTextEdit>
class StudyCardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StudyCardWidget(QWidget *parent = nullptr);

signals:

private:
    QTextEdit* header;
    QTextEdit* body;
};

#endif // STUDYCARD_H
