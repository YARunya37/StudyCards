#ifndef RICHTEXTEDIT_H
#define RICHTEXTEDIT_H

#include <QTextEdit>
#include <QMimeData>
#include <QImage>
#include <QTextCursor>
#include <QTextImageFormat>

class RichTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit RichTextEdit(QWidget *parent = nullptr);

protected:
    // Переопределяем этот метод для обработки вставки (Drag&Drop или Ctrl+V)
    void insertFromMimeData(const QMimeData *source) override;

private:
    // Вспомогательный метод для вставки картинки
    void InsertImageToEditor(const QImage &image);
};

#endif // RICHTEXTEDIT_H
