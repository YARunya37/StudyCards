#ifndef RICHTEXTEDIT_H
#define RICHTEXTEDIT_H

#include <QTextEdit>
#include <QMimeData>
#include <QImage>
#include <QTextCursor>
#include <QTextImageFormat>

class QMouseEvent;
class QPaintEvent;
class QEvent;

// Редактор поверх QTextEdit: вставка изображений и изменение их размера
// перетаскиванием угловой ручки (в стиле Obsidian).
class RichTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit RichTextEdit(QWidget *parent = nullptr);

protected:
    // Обработка вставки (Drag&Drop или Ctrl+V)
    void insertFromMimeData(const QMimeData *source) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    // Вспомогательный метод для вставки картинки
    void InsertImageToEditor(const QImage &image);

    // Курсор, нормализованный строго на символ-объект изображения в блоке,
    // либо невалидный курсор, если объекта в блоке нет
    QTextCursor NormalizedImageCursor(const QTextCursor &cursor) const;
    // Изображение в точке pos: невалидный курсор, если точки внутри картинки нет
    QTextCursor ImageCursorAt(const QPoint &pos) const;
    // То же, но с гистерезисом: пока курсор внутри последнего hover-прямоугольника,
    // считаем, что он над изображением (иначе ручка «убегает» у границ)
    QTextCursor ResolveImageAt(const QPoint &pos) const;

    QRect ImageViewportRect(const QTextCursor &cursor) const;
    QRect HandleRect(const QRect &imageRect) const;
    QRect HandleHitRect(const QRect &imageRect) const;
    QImage DecodeImage(const QTextImageFormat &format) const;

    QTextCursor m_imageCursor;  // изображение, которое сейчас ресайзим
    QTextCursor m_hoverCursor;  // изображение под курсором
    QRect m_previewRect;        // «резиновая рамка» во время перетаскивания
    QRect m_hoverRect;          // прямоугольник изображения под курсором
    bool m_resizing = false;
    QPoint m_dragStart;
    int m_startWidth = 0;
    int m_startHeight = 0;
};

#endif // RICHTEXTEDIT_H
