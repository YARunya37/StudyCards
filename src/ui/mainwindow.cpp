#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/FileLoader.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Создаём центральный виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Создаём компоновку
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    // Кнопка "Открыть файл"
    QPushButton *btnOpen = new QPushButton("📂 Открыть документ (DOCX/MD)", this);

    // Текстовый редактор
    editor = new ScaledTextEdit(this);
    editor->setPlaceholderText("Нажми кнопку чтобы открыть файл...");
    editor->setReadOnly(false);
    editor->setAcceptRichText(true);

    // Добавляем в компоновку
    layout->addWidget(btnOpen);
    layout->addWidget(editor, 1);  // 1 = растягивается

    // Подключаем сигнал кнопки к слоту
    connect(btnOpen, &QPushButton::clicked, this, &MainWindow::onOpenFile);

    setWindowTitle("StudyCards - FileLoader Test");
    resize(900, 700);

    qInfo() << "MainWindow: Initialized";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onOpenFile()
{
   qInfo() << "MainWindow: Opening";

    // 1. Получаем путь к приложению
    QString appDir = QCoreApplication::applicationDirPath();

    // 2. Путь к pandoc (относительно exe файла)
    QString pandocPath = QDir::cleanPath(appDir + "/utils/pandoc/pandoc.exe");

    // 3. Проверяем что pandoc существует
    if (!QFile::exists(pandocPath)) {
        QMessageBox::critical(
            this, "Ошибка",
            "Pandoc не найден!\n\n"
            "Путь: " + pandocPath + "\n\n"
            "Убедитесь что:\n"
            "1. pandoc.exe находится в папке utils/pandoc/\n"
            "2. Проект был правильно собран\n"
            "3. Папка utils/pandoc скопирована в build/"
        );
        qWarning() << "MainWindow: Pandoc not found!" << pandocPath;
        return;
    }

    // 4. Диалог выбора файла
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Открыть документ",
        "",
        "Documents (*.docx *.md *.markdown);;All Files (*)"
    );

    if (filePath.isEmpty()) {
        qInfo() << "MainWindow: User cancelled file dialog";
        return;
    }

    qInfo() << "Selected file:" << filePath;
    qInfo() << "File exists:" << QFile::exists(filePath);

    // 5. Показываем статус загрузки
    editor->setPlainText("Загрузка:\n" + filePath + "\n\nПодождите...");

    // 6. FileLoader возвращает уже готовый HTML с CSS
    QString html;
    if (loadDocument(filePath, html, pandocPath)) {
        qInfo() << "MainWindow: File loaded successfully!";
        qInfo() << "HTML size:" << html.size() << "bytes";

        editor->setHtml(html);
    } else {
        qWarning() << "MainWindow: Failed to load file:" << filePath;

        QMessageBox::critical(
            this, "Ошибка",
            "Не удалось загрузить файл!\n\n"
            "Файл: " + filePath + "\n\n"
            "Проверьте:\n"
            "1. Файл существует и не повреждён\n"
            "2. Pandoc.exe находится в правильном месте\n"
            "3. Формат файла поддерживается (DOCX или MD)\n\n"
            "Смотрите вывод в консоли для деталей"
        );
        editor->setPlainText("");
    }

    qInfo() << "=== MainWindow: onOpenFile complete ===";
}
