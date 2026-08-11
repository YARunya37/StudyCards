#include "richtextedit.h"
#include <QBuffer>
#include <QImage>
#include <QUrl>
#include <QTextCursor>
#include <QVariant>
#include <QDebug>

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



}
