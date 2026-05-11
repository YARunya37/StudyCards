#include "documentui.h"

#include <QMainWindow>
#include <QTextEdit>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>

#include "filetreewidget.h"
#include "documentmanager.h"

DocumentUI::DocumentUI(
    QMainWindow* mainWindow,
    FileTreeWidget* fileTree,
    QTextEdit* textEditor,
    DocumentManager* docManager,
    QObject* parent)
    : QObject(parent)
    , m_mainWindow(mainWindow)
    , m_fileTree(fileTree)
    , m_textEditor(textEditor)
    , m_docManager(docManager)
    , m_isLoading(false)
{
    // Подключаем сигналы DocumentManager
    connect(m_docManager, &DocumentManager::contentLoaded,
            this, &DocumentUI::onContentLoaded);

    connect(m_docManager, &DocumentManager::modificationChanged,
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
    auto reply = QMessageBox::question(
        m_mainWindow, "Сохранить?",
        "Сохранить изменения?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (reply == QMessageBox::Save) {
        return m_docManager->saveDocument();
    }
    return reply == QMessageBox::Discard;
}

void DocumentUI::onFileDoubleClicked(const QString& fileName)
{
    if (m_docManager->isModified()) {
        if (!confirmSaveChanges()) return;
    }
    m_docManager->loadDocument(fileName);
}

void DocumentUI::onTextChanged()
{
    if (m_isLoading) return;
    // Извлекаем только содержимое body!
    QString html = m_textEditor->toHtml();
    html = extractBodyContent(html);

    m_docManager->setContent(html);
}

QString DocumentUI::extractBodyContent(const QString& html)
{
    int bodyStart = html.indexOf("<body");
    if (bodyStart == -1) return html;

    bodyStart = html.indexOf(">", bodyStart) + 1;
    int bodyEnd = html.indexOf("</body>", bodyStart);

    if (bodyEnd == -1) return html;

    return html.mid(bodyStart, bodyEnd - bodyStart);
}

void DocumentUI::onContentLoaded(const QString& content)
{
    // Игнорируем изменения
    m_isLoading = true;

    if (content.contains("<")) {
        // Если это HTML
        m_textEditor->setHtml(content);
    } else {
        m_textEditor->setPlainText(content);
    }
    // Разрешаем отслеживание
    m_isLoading = false;
}

void DocumentUI::onModificationChanged(bool modified)
{
    updateWindowTitle(m_docManager->currentFileName(), modified);
}

void DocumentUI::onFileSave()
{
    m_docManager->saveDocument();
}
