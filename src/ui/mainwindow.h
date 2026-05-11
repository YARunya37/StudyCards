#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTextEdit>
#include "textformattingtoolbar.h"

#include <QMainWindow>
#include <QSplitter>
#include <QString>

class QLabel;
class FileTreeWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void setTicketGroupName(const QString& name);

private:
    Ui::MainWindow *ui;
    QTextEdit* sourceTextWidget;
    QWidget* toolbarPanel;
    QLabel* ticketGroupLabel;
    TextFormattingToolbar* textToolbar;

    // Метод для создания разделителя между основными виджетами
    void SetUpSPlitter();
};

#endif // MAINWINDOW_H
