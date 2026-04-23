#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "scaledtextedit.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onOpenFile();

private:
    Ui::MainWindow *ui;
    ScaledTextEdit *editor;
};

#endif // MAINWINDOW_H
