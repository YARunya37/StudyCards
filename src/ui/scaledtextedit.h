#ifndef SCALEDTEXTEDIT_H
#define SCALEDTEXTEDIT_H

#include <QTextEdit>
#include <QWheelEvent>

class ScaledTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    ScaledTextEdit(QWidget *parent = nullptr) : QTextEdit(parent) {}

protected:
    void wheelEvent(QWheelEvent *event) override
    {
        // Ctrl + колёсико = масштабирование
        if (event->modifiers() == Qt::ControlModifier) {
            QFont font = this->font();
            int size = font.pointSize();

            if (event->angleDelta().y() > 0) {
                size += 1;
            } else {
                size -= 1;
            }

            size = qBound(8, size, 72);
            font.setPointSize(size);
            this->setFont(font);

            event->accept();  // Блокируем прокрутку
            return;
        }

        // Обычная прокрутка
        QTextEdit::wheelEvent(event);
    }
};

#endif
