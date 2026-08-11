#include "richtextedit.h"
#include <QBuffer>
#include <QImage>
#include <QUrl>
#include <QTextCursor>
#include <QVariant>
namespace
{
    // Ограничения вставки: не раздувать base64 и помещаться в окно
    constexpr int MaxInsertWidth = 1200;
    constexpr int MaxInsertHeight = 900;
    constexpr int DefaultWidth = 800;

}

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



}
