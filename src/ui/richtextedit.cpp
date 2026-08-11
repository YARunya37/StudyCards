#include "richtextedit.h"
#include <QBuffer>
#include <QImage>
#include <QUrl>
#include <QTextCursor>
#include <QVariant>

RichTextEdit::RichTextEdit(QWidget *parent) : QTextEdit(parent)
{
    // Устанавливаем стили для изображений
    this->document()->setDefaultStyleSheet("img { max-width: 100%; height: auto; }");
}

void RichTextEdit::insertFromMimeData(const QMimeData *source)
{
    // Если в буфере есть картинка (Ctrl+V)
    if (source->hasImage()) {
        QImage image = qvariant_cast<QImage>(source->imageData());
        insertImageToEditor(image);
        return;
    }

    // Если перетаскиваем файл (Drag & Drop)
    if (source->hasUrls()) {
        QList<QUrl> urls = source->urls();
        for (const QUrl &url : urls) {
            QString localFile = url.toLocalFile();
            if (!localFile.isEmpty()) {
                QImage image(localFile);
                if (!image.isNull()) {
                    // Это картинка -> вставляем её
                    insertImageToEditor(image);
                } else {
                    // Это не картинка (например, текст) -> стандартное поведение
                    QTextEdit::insertFromMimeData(source);
                }
                return; // Обработали, выходим
            }
        }
    }

    // Обычный текст
    QTextEdit::insertFromMimeData(source);
}

void RichTextEdit::InsertImageToEditor(const QImage &image)
{
    // Определяем максимальную ширину (но не уменьшаем маленькие изображения)
    int maxWidth = qMin(this->viewport()->width() - 40, 1200);  // Увеличили макс. ширину до 1200px
    if (maxWidth < 100) maxWidth = 800;

    // Масштабируем только если изображение слишком большое
    QImage processedImage = image;
    if (image.width() > maxWidth) {
        processedImage = image.scaled(
            maxWidth,
            900,  // Максимальная высота
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation  // Качественное сглаживание
        );
    }

    // Сохраняем в PNG для максимального качества (без потерь)
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    processedImage.save(&buffer, "PNG");

    // Конвертируем в base64
    QString base64Image = QString("data:image/png;base64,%1")
        .arg(QString(byteArray.toBase64()));

    // Вычисляем размеры для отображения
    int displayWidth = qMin(processedImage.width(), maxWidth);

    // Вставляем с ограничениями по размеру
    QTextCursor cursor = textCursor();
    QString html = QString("<img src=\"%1\" style=\"max-width: %2px; width: %2px; height: auto;\" />")
        .arg(base64Image)
        .arg(displayWidth);

    cursor.insertHtml(html);
}
