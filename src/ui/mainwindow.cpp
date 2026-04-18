#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/FileLoader.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QWheelEvent>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    editor = new ScaledTextEdit(this);
    editor->setReadOnly(false);     // ← Разрешить редактирование!
    editor->setAcceptRichText(true); // ← Принимать форматированный текст
    layout->addWidget(btnOpen);
    layout->addWidget(editor);
    connect(btnOpen, &QPushButton::clicked, this, &MainWindow::onOpenFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onOpenFile()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString pandocPath = QDir::cleanPath(appDir + "/../../src/utils/pandoc/pandoc.exe");

    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Открыть документ",
        "",
        "Documents (*.docx *.md *.markdown);;All Files (*)"
    );

    if (filePath.isEmpty()) {
        return;
    }

    editor->setPlainText("Загрузка: " + filePath + "...");

    std::string html;
    if (loadDocument(filePath.toStdString(), html, pandocPath.toStdString())) {
        QString htmlContent = QString::fromStdString(html);

        //CSS для таблиц
        QString cssStyles =
            "<style>"
            "table { "
            "    border-collapse: collapse; "
            "    width: 100%; "
            "    border: 2px solid #888; "
            "}"
            "th { "
            "    padding: 6px; "
            "    font-weight: bold; "
            "}"
            "td { "
            "    border: 1px solid #888; "
            "    padding: 6px; "
            "}"
            "img { "
                "    max-width: 40em; "
                "    width: 100%; "
                "    height: auto; "
                "    display: block; "
                "    margin: 1em 0; "
                "}"
            "pre { background: #1e1e1e; padding: 10px; overflow-x: auto; }"
            "code { font-family: 'Courier New', monospace; }"
            "a { color: #9b59b6; text-decoration: underline; }"
            "</style>";
        editor->setHtml(cssStyles + htmlContent);
    } else {
        QMessageBox::critical(this, "Ошибка",
            "Не удалось загрузить файл!\n\n"
            "Проверь:\n"
            "1. Файл существует\n"
            "2. Pandoc.exe находится в src/utils/pandoc/");
        editor->setPlainText("");
    }
}

