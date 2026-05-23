#ifndef BASE_QUESTION_H
#define BASE_QUESTION_H

#include <QStackedWidget>
#include "studycard.h"

class Question : public QStackedWidget
{
    Q_OBJECT
public:
    explicit Question(StudyCardWidget *referenceCard, QWidget *parent = nullptr);

    virtual ~Question() = default;

    // Виртуальный метод для клонирования вопроса
    virtual Question* Clone() const = 0;
protected:

signals:
    void sendAnswer(bool answer);

protected:
    // Метод для показа обратной стороны
    virtual void ShowBack();



    // Указатель на StudyCardWidget
    StudyCardWidget *refCard;

    // Виджеты для страниц (будут настраиваться в дочерних классах)
    QWidget *face;
    QWidget *back;

    // Поле ответа
    bool answer;
};

#endif // BASE_QUESTION_H
