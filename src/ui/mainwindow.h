#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "scaledtextedit.h"

#include <QMainWindow>
#include <QSplitter>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QPushButton>

class QComboBox;
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
private slots:
    void toggleBold();
    void toggleItalic();
    void toggleUnderline();

    void alignLeft();
    void alignCenter();
    void alignRight();
    void alignJustify();

    void changeTextColor();
    void changeHighlightColor();

    void insertBulletList();
    void insertNumberedList();

private:
    Ui::MainWindow *ui;
    ScaledTextEdit* sourceTextWidget;
    QWidget* textEditorContainer;

    QPushButton* btnBold;
    QPushButton* btnItalic;
    QPushButton* btnUnderline;
    QPushButton* btnTextColor;
    QPushButton* btnHighlightColor;
    QComboBox* comboFontSize;
    QLabel* ticketGroupLabel;
    // Метод для создания разделителя между основными виджетами
    void SetUpSPlitter();
    void createToolbar();
};

#endif // MAINWINDOW_H
