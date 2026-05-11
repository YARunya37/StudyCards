#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "scaledtextedit.h"
#include "textformattingtoolbar.h"

#include <QMainWindow>
#include <QSplitter>
#include <QString>
#include <QStringList>
#include <QFileDialog>

class QLabel;

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
    QWidget* tempGroupContainer;
    QWidget* midContentContainer;
    QLabel* ticketGroupLabel;


    // Метод для создания разделителя между основными виджетами
    void SetUpSPlitter();
};

#endif // MAINWINDOW_H
