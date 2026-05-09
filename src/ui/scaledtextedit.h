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

    qreal getZoomFactor() const;
    void setZoomFactor(qreal factor);
    void resetZoom();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    qreal zoomFactor;
    void applyZoomToAllText();  // ← Новая функция
};
#endif // SCALEDTEXTEDIT_H
