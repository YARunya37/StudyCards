#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "scaledtextedit.h"

#include <QMainWindow>
#include <QSplitter>
#include <QString>
#include <QStringList>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    // Метод для создания разделителя между основными виджетами
    void SetUpSPlitter();
};

#endif // MAINWINDOW_H
