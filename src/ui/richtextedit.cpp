#include "richtextedit.h"

#include <QBuffer>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QTextBlock>
#include <QUrl>
#include <QVariant>
#include <QDebug>

namespace
{
    // Ограничения вставки: не раздувать base64 и помещаться в окно
    constexpr int MaxInsertWidth = 1200;
    constexpr int MaxInsertHeight = 900;
    constexpr int DefaultWidth = 800;

    constexpr int HandleSize = 12;
    constexpr int HandleMargin = 4;
    // Зона клика больше видимой ручки — её реально можно ухватить
    constexpr int HandleHitMargin = 10;
}

RichTextEdit::RichTextEdit(QWidget *parent) : QTextEdit(parent)
{
    // Картинки не должны вылезать за видимую область редактора
    this->document()->setDefaultStyleSheet("img { max-width: 100%; height: auto; }");
    // События движения мыши без кнопок — чтобы показывать ручку при наведении
    viewport()->setMouseTracking(true);
}

void RichTextEdit::insertFromMimeData(const QMimeData *source)
{
    // Картинка в буфере (Ctrl+V), в т.ч. из внешних программ (Photoshop и т.п.)
    if (source->hasImage())
    {
        InsertImageToEditor(qvariant_cast<QImage>(source->imageData()));
        return;
    }

    if (source->hasUrls())
    {
        bool handledAnyFile = false;
        const QList<QUrl> urls = source->urls();
        for (const QUrl &url : urls)
        {
            const QString localFile = url.toLocalFile();
            if (localFile.isEmpty())
            {
                continue;
            }

            handledAnyFile = true;
            QImage image(localFile);
            if (!image.isNull())
            {
                InsertImageToEditor(image);
            }
            else
            {
                // Файл-не-картинка иначе вставился бы в текст как путь file://;
                // такие «ссылки» в конспекте бесполезны, поэтому пропускаем
                qWarning() << "RichTextEdit: non-image file skipped:" << localFile;
            }
        }

        // Ни одного локального файла (например, перетащили ссылку из браузера) —
        // сохраняем прежнее стандартное поведение
        if (!handledAnyFile)
        {
            QTextEdit::insertFromMimeData(source);
        }
        return;
    }

    QTextEdit::insertFromMimeData(source);
}

void RichTextEdit::InsertImageToEditor(const QImage &image)
{
    int maxWidth = qMin(this->viewport()->width() - 40, MaxInsertWidth);
    if (maxWidth < 100)
    {
        maxWidth = DefaultWidth;
    }

    QImage processedImage = image;
    if (image.width() > maxWidth)
    {
        processedImage = image.scaled(maxWidth, MaxInsertHeight,
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation);
    }

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    processedImage.save(&buffer, "PNG");

    QTextImageFormat imageFormat;
    imageFormat.setName(QString("data:image/png;base64,%1").arg(QString(byteArray.toBase64())));
    // Фиксируем размер в формате изображения, а не в style-атрибуте:
    // тогда toHtml() записывает width/height, и после повторного открытия
    // документа картинка сохраняет заданный размер
    imageFormat.setWidth(processedImage.width());
    imageFormat.setHeight(processedImage.height());

    textCursor().insertImage(imageFormat);
}

QTextCursor RichTextEdit::NormalizedImageCursor(const QTextCursor &cursor) const
{
    const QTextBlock block = cursor.block();
    const QString text = block.text();
    const int posInBlock = cursor.position() - block.position();

    // Нечёткий хит-тест ставит курсор либо НА объект, либо сразу ПОСЛЕ него —
    // приводим к единой позиции: символ-объект изображения
    int objectIndex = -1;
    if (posInBlock < text.length() && text.at(posInBlock) == QChar::ObjectReplacementCharacter)
    {
        objectIndex = posInBlock;
    }
    else if (posInBlock > 0 && text.at(posInBlock - 1) == QChar::ObjectReplacementCharacter)
    {
        objectIndex = posInBlock - 1;
    }
    else
    {
        objectIndex = text.indexOf(QChar::ObjectReplacementCharacter);
    }

    if (objectIndex < 0)
    {
        return QTextCursor();
    }

    QTextCursor normalized = cursor;
    normalized.clearSelection();
    normalized.setPosition(block.position() + objectIndex);
    return normalized;
}

QTextCursor RichTextEdit::ImageCursorAt(const QPoint &pos) const
{
    QTextCursor cursor = cursorForPosition(pos);
    if (cursor.isNull() || !cursor.charFormat().isImageFormat())
    {
        return QTextCursor();
    }

    // cursorForPosition «дотягивается» до ближайшего символа, поэтому пустое
    // место справа от картинки или ниже текста тоже падает в изображение.
    // Доверяем хиту, только если точка реально внутри видимого прямоугольника.
    const QRect imageRect = ImageViewportRect(cursor);
    if (!imageRect.adjusted(-2, -2, 2, 2).contains(pos))
    {
        return QTextCursor();
    }
    return NormalizedImageCursor(cursor);
}

QTextCursor RichTextEdit::ResolveImageAt(const QPoint &pos) const
{
    const QTextCursor cursor = ImageCursorAt(pos);
    if (!cursor.isNull())
    {
        return cursor;
    }

    // Гистерезис: у самых границ картинки хит-тест может мигать,
    // поэтому пока курсор внутри последнего hover-прямоугольника —
    // он всё ещё «над изображением»
    if (!m_hoverCursor.isNull() && m_hoverRect.isValid() && m_hoverRect.contains(pos))
    {
        return m_hoverCursor;
    }
    return QTextCursor();
}

QRect RichTextEdit::ImageViewportRect(const QTextCursor &cursor) const
{
    if (cursor.isNull() || !cursor.charFormat().isImageFormat())
    {
        return QRect();
    }

    // Курсор мог встать до ИЛИ после объекта — cursorRect для «после» даёт
    // правый край, и рамка уезжала на ширину картинки. Нормализуем.
    const QTextCursor anchorCursor = NormalizedImageCursor(cursor);
    if (anchorCursor.isNull())
    {
        return QRect();
    }

    // cursorRect — собственный маппинг координат Qt: уже в системе viewport,
    // учитывает центрирование, отступы и скролл
    const QRect anchor = cursorRect(anchorCursor);

    const QTextImageFormat format = cursor.charFormat().toImageFormat();
    int width = static_cast<int>(format.width());
    if (width <= 0)
    {
        // Старые документы хранили размер только в style — берём исходный
        width = DecodeImage(format).width();
    }
    return QRect(anchor.x(), anchor.y(), width, anchor.height());
}

QRect RichTextEdit::HandleRect(const QRect &imageRect) const
{
    return QRect(imageRect.right() - HandleSize - HandleMargin,
                 imageRect.bottom() - HandleSize - HandleMargin,
                 HandleSize,
                 HandleSize);
}

QRect RichTextEdit::HandleHitRect(const QRect &imageRect) const
{
    return HandleRect(imageRect).adjusted(-HandleHitMargin, -HandleHitMargin,
                                          HandleHitMargin, HandleHitMargin);
}

void RichTextEdit::mouseMoveEvent(QMouseEvent *event)
{
    if (m_resizing)
    {
        UpdateResize(event->pos());
        return;
    }

    const QTextCursor cursor = ResolveImageAt(event->pos());
    if (!cursor.isNull() && !isReadOnly())
    {
        m_hoverRect = ImageViewportRect(cursor);
        m_hoverCursor = cursor;
        viewport()->setCursor(HandleHitRect(m_hoverRect).contains(event->pos())
                                  ? Qt::SizeFDiagCursor
                                  : Qt::IBeamCursor);
    }
    else
    {
        m_hoverRect = QRect();
        m_hoverCursor = QTextCursor();
        viewport()->setCursor(Qt::IBeamCursor);
    }
    viewport()->update();
    QTextEdit::mouseMoveEvent(event);
}

void RichTextEdit::leaveEvent(QEvent *event)
{
    m_hoverRect = QRect();
    m_hoverCursor = QTextCursor();
    viewport()->update();
    QTextEdit::leaveEvent(event);
}


void RichTextEdit::paintEvent(QPaintEvent *event)
{
    QTextEdit::paintEvent(event);

    QPainter painter(viewport());

    if (m_resizing && m_previewRect.isValid())
    {
        // Тянем только рамку: размер в документе меняется один раз при отпускании,
        // иначе каждый mouseMove пересчитывал бы layout всего документа
        painter.setPen(QPen(QColor(0, 120, 215), 2));
        painter.drawRect(m_previewRect);
        painter.setBrush(QColor(0, 120, 215));
        painter.drawRect(HandleRect(m_previewRect));
        return;
    }

    if (!m_hoverRect.isValid() || isReadOnly())
    {
        return;
    }
    painter.setPen(QPen(QColor(0, 120, 215, 220), 2));
    painter.drawRect(m_hoverRect);
    painter.setBrush(QColor(0, 120, 215));
    painter.drawRect(HandleRect(m_hoverRect));
}

QImage RichTextEdit::DecodeImage(const QTextImageFormat &format) const
{
    const QString name = format.name();
    const int marker = name.indexOf("base64,");
    if (marker < 0)
    {
        return QImage(name);
    }
    return QImage::fromData(QByteArray::fromBase64(name.mid(marker + 7).toLatin1()));
}
