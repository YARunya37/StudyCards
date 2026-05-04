#include "scaledtextedit.h"
#include <QFont>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QTextCursor>
#include <QDesktopServices>
#include <QDebug>
#include <QTextBlock>
#include <QTextCharFormat>

ScaledTextEdit::ScaledTextEdit(QWidget *parent)
    : QTextEdit(parent), zoomFactor(1.0)
{
    // Разрешаем и редактирование, и ссылки
    setTextInteractionFlags(Qt::TextEditorInteraction | Qt::LinksAccessibleByMouse);

    // Устанавливаем базовый шрифт
    QFont baseFont = this->font();
    baseFont.setPointSize(12);
    setFont(baseFont);
    document()->setDefaultFont(baseFont);

    setWordWrapMode(QTextOption::WordWrap);
}

void ScaledTextEdit::wheelEvent(QWheelEvent *event)
{
    // Ctrl + колёсико = масштабирование
    if (event->modifiers() & Qt::ControlModifier) {

        // Вычисляем новый масштаб
        if (event->angleDelta().y() > 0) {
            zoomFactor *= 1.1;
        } else {
            zoomFactor /= 1.1;
        }

        zoomFactor = qBound(0.25, zoomFactor, 5.0);

        // Применяем масштаб ВСЕМУ тексту:
        applyZoomToAllText();

        event->accept();
        return;
    }

    // Обычная прокрутка
    QTextEdit::wheelEvent(event);
}

void ScaledTextEdit::applyZoomToAllText()
{
    // Базовый размер шрифта
    const int BASE_SIZE = 12;
    int newSize = qRound(BASE_SIZE * zoomFactor);
    newSize = qBound(8, newSize, 72);

    // 1. Меняем шрифт виджета (для нового текста)
    QFont widgetFont = this->font();
    widgetFont.setPointSize(newSize);
    setFont(widgetFont);

    // 2. Меняем шрифт по умолчанию в документе
    QTextDocument* doc = document();
    QFont defaultFont = doc->defaultFont();
    defaultFont.setPointSize(newSize);
    doc->setDefaultFont(defaultFont);

    // 3. ВАЖНО: Масштабируем ВСЕ существующие форматы символов!
    QTextBlock block = doc->firstBlock();
    while (block.isValid()) {
        QTextBlock::iterator it;
        for (it = block.begin(); !(it.atEnd()); ++it) {
            QTextFragment fragment = it.fragment();
            if (fragment.isValid()) {
                QTextCharFormat fmt = fragment.charFormat();
                QFont font = fmt.font();

                // Сохраняем стиль (bold/italic/underline), меняем только размер
                font.setPointSize(newSize);
                fmt.setFont(font);

                // Применяем изменённый формат обратно
                QTextCursor cursor(doc);
                cursor.setPosition(fragment.position());
                cursor.setPosition(fragment.position() + fragment.length(), QTextCursor::KeepAnchor);
                cursor.mergeCharFormat(fmt);
            }
        }
        block = block.next();
    }
    // 4. Принудительно обновляем отображение
    viewport()->update();
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


qreal ScaledTextEdit::getZoomFactor() const
{
    return zoomFactor;
}

void ScaledTextEdit::setZoomFactor(qreal factor)
{
    zoomFactor = qBound(0.25, factor, 5.0);
    applyZoomToAllText();
}

void ScaledTextEdit::resetZoom()
{
    zoomFactor = 1.0;
    applyZoomToAllText();
}
