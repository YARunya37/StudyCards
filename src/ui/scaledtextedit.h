#ifndef SCALEDTEXTEDIT_H
#define SCALEDTEXTEDIT_H

#include <QTextEdit>
#include <QWheelEvent>
#include <QMouseEvent>

class ScaledTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit ScaledTextEdit(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // SCALEDTEXTEDIT_H
