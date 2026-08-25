#include "documentui.h"

#include <QMainWindow>
#include <QTextEdit>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>

#include "filetreewidget.h"
#include "filemanager.h"

DocumentUI::DocumentUI(
    QMainWindow* mainWindow,
    FileTreeWidget* fileTree,
    QTextEdit* textEditor,
    FileManager* fileManager,
    QObject* parent)
    : QObject(parent)
    , m_mainWindow(mainWindow)
    , m_fileTree(fileTree)
    , m_textEditor(textEditor)
    , m_fileManager(fileManager)
    , m_isLoading(true)
{
    // Подключаем сигналы FileManager
    connect(m_fileManager, &FileManager::modificationChanged,
            this, &DocumentUI::onModificationChanged);

    // Подключаем сигнал двойного клика
    connect(m_fileTree, &FileTreeWidget::fileDoubleClicked,
            this, &DocumentUI::onFileDoubleClicked);

    // Подключаем изменение текста
    connect(m_textEditor, &QTextEdit::textChanged,
            this, &DocumentUI::onTextChanged);
}

void DocumentUI::connectMenuActions(QAction* actionSave)
{
    connect(actionSave, &QAction::triggered, this, &DocumentUI::onFileSave);
}

bool DocumentUI::CanClose()
{
    return confirmSaveChanges();
}

void DocumentUI::updateWindowTitle(const QString& fileName, bool isModified)
{
    QString title = "StudyCards";

    if (!fileName.isEmpty()) {
        title += " - " + fileName;
    }

    if (isModified) {
        title += " [*]";
    }

    m_mainWindow->setWindowTitle(title);
}

bool DocumentUI::confirmSaveChanges()
{
    // Проверка: есть ли текущий файл?
    QString currentFile = m_fileManager->currentFile();
    if (currentFile.isEmpty()) {
        // Нет открытого файла — просто возвращаем true (продолжаем)
        return true;
    }
    // Проверка: есть ли изменения?
    if (!m_fileManager->isModified()) {
            return true;  // Нет изменений — продолжаем
        }

    auto reply = QMessageBox::question(
            m_mainWindow, "Сохранить?",
            "Сохранить изменения в \"" + currentFile + "\"?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (reply == QMessageBox::Save) {
        QString html = m_textEditor->toHtml();
        bool saved = m_fileManager->saveDocument(currentFile, html);

        if (saved) {
            m_fileManager->setModified(false);
        }

        return saved;
    }

    if (reply == QMessageBox::Discard) {
        m_fileManager->setModified(false);  // Сбрасываем флаг
        return true;
    }

    return false;
}

void DocumentUI::onFileDoubleClicked(const QString& fileName)
{
    // Сначала проверяем текущий файл
      if (!m_fileManager->currentFile().isEmpty() && m_fileManager->isModified()) {
          if (!confirmSaveChanges()) {
              return;  // Пользователь нажал Cancel
          }
      }

    // Загружаем новый файл
    QString content;
    if (m_fileManager->loadDocument(fileName, content)) {
        m_isLoading = true;

        //Для переноса кода(чтобы не уходил за другой виджет)
        QString css = R"(
                <style>
                    pre, pre.sourceCode, div.sourceCode {
                        white-space: pre-wrap !important;
                        word-wrap: break-word !important;
                        word-break: break-all !important;
                        overflow-x: auto !important;
                        max-width: 100% !important;
                        background: #f5f5f5 !important;
                        color: #333 !important;
                    }
                    code {
                        white-space: pre-wrap !important;
                        word-wrap: break-word !important;
                        color: #333 !important;
                        background: transparent !important;
                    }
                </style>
                )";

        // Вставляем CSS в начало content
        if (content.contains("</head>", Qt::CaseInsensitive)) {
            content.replace("</head>", css + "</head>", Qt::CaseInsensitive);
        } else {
            content = css + content;
        }

        m_textEditor->setHtml(content);
        m_textEditor->setReadOnly(false);
        m_isLoading = false;

        m_fileManager->setCurrentFile(fileName);
    } else {
        QMessageBox::warning(m_mainWindow, "Ошибка",
                             "Не удалось загрузить файл: " + fileName);
    }
}

void DocumentUI::onTextChanged()
{
    // Игнорируем программные изменения
    if (m_isLoading) return;

    // Устанавливаем флаг modified
    m_fileManager->setModified(true);
}

void DocumentUI::onModificationChanged(bool modified)
{
    updateWindowTitle(m_fileManager->currentFile(), modified);
}

void DocumentUI::onFileSave()
{
    QString currentFile = m_fileManager->currentFile();
    if (currentFile.isEmpty()) {
        QMessageBox::warning(m_mainWindow, "Ошибка", "Нет открытого файла");
        return;
    }

    // Получаем HTML из редактора
    QString html = m_textEditor->toHtml();

    // Используем FileManager.saveDocument()
    if (m_fileManager->saveDocument(currentFile, html)) {
        m_fileManager->setModified(false);
        QMessageBox::information(m_mainWindow, "Успех", "Файл сохранён");
    } else {
        QMessageBox::warning(m_mainWindow, "Ошибка", "Не удалось сохранить файл");
    }
}
