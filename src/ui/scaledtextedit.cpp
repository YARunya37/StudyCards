#include "scaledtextedit.h"
#include <QFont>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QTextCursor>
#include <QDesktopServices>
#include <QDebug>

ScaledTextEdit::ScaledTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    // Включаем взаимодействие с текстом
    setTextInteractionFlags(Qt::TextBrowserInteraction);
}

void ScaledTextEdit::wheelEvent(QWheelEvent *event)
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

void ScaledTextEdit::mousePressEvent(QMouseEvent *event)
{
    // Проверяем клик по ссылке (левая кнопка мыши)
    if (event->button() == Qt::LeftButton) {
        QTextCursor cursor = cursorForPosition(event->pos());
        QString anchor = cursor.charFormat().anchorHref();

        if (!anchor.isEmpty()) {
            qInfo() << "Opening link:" << anchor;
            QDesktopServices::openUrl(QUrl(anchor));
            event->accept();
            return;
        }
    }

    // Обычная обработка клика
    QTextEdit::mousePressEvent(event);
}
