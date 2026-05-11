#ifndef TEMPCARDSWIDGET_H
#define TEMPCARDSWIDGET_H

#include <QTabWidget>
#include <QWidget>

class TempCardsWidget : public QTabWidget
{
    Q_OBJECT
public:
    TempCardsWidget(QWidget* parent = nullptr);

private:
    // UI при первом запуске
    void setupInitUI();
};

#endif // TEMPCARDSWIDGET_H
