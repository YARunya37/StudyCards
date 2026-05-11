#include "tempcardswidget.h"
#include <QTextBrowser>
TempCardsWidget::TempCardsWidget(QWidget* parent) :
    QTabWidget(parent)
{
    setupInitUI();
}

void TempCardsWidget::setupInitUI()
{
    QTextBrowser* hint = new QTextBrowser(this);
    hint->setText("Выберите группу через меню \"Группы билетов\"");
    hint->setAlignment(Qt::AlignCenter);

    QFont hintFont = QFont();
    hintFont.setPointSize(16);
    hint->setFont(hintFont);

    addTab(hint, "");
}
