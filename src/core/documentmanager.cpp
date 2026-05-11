#include "documentmanager.h"
#include "filemanager.h"
#include "FileLoader.h"

#include <QTextDocument>

DocumentManager::DocumentManager(FileManager* fileManager, QObject* parent)
    : QObject(parent)
    , m_fileManager(fileManager)
    , m_isModified(false)
    , m_isHtmlFile(false)
{
}

bool DocumentManager::loadDocument(const QString& fileName)
{
    if (!m_fileManager) return false;

    QString filePath = m_fileManager->getFilePath(fileName);
    m_isHtmlFile = false;
    QString content;

    // Проверяем — это HTML или исходный файл
    if (m_isHtmlFile || fileName.endsWith(".html", Qt::CaseInsensitive)) {
        // Это HTML — загружаем как есть
        content = m_fileManager->get_file_content(fileName);
        m_isHtmlFile = true;
    }
    else if (filePath.endsWith(".docx", Qt::CaseInsensitive) ||
             filePath.endsWith(".doc", Qt::CaseInsensitive) ||
             filePath.endsWith(".md", Qt::CaseInsensitive) ||
             filePath.endsWith(".markdown", Qt::CaseInsensitive)) {

        // Это исходный файл — конвертируем через FileLoader
        QString htmlContent;
        if (::loadDocument(filePath, htmlContent, getPandocPath())) {
            // Добавляем CSS
            addCssToHtml(htmlContent);
            content = htmlContent;
            m_isHtmlFile = true;
        } else {
            content = "[Не удалось загрузить файл]";
            m_isHtmlFile = false;
            return false;
        }
    } else {
        // Это txt — читаем как текст
        content = m_fileManager->get_file_content(fileName);
        m_isHtmlFile = false;
    }

    if (content.isEmpty() && !m_fileManager->is_file(fileName)) {
        return false;
    }

    m_currentFileName = fileName;
    m_content = content;
    m_isModified = false;

    emit contentLoaded(content);
    emit currentDocumentChanged(fileName);
    emit modificationChanged(false);

    return true;
}

bool DocumentManager::saveDocument()
{
    if (m_currentFileName.isEmpty() || !m_fileManager) {
        return false;
    }

    m_fileManager->write_to_file(m_currentFileName, m_content);
    m_isModified = false;

    emit documentSaved();
    emit modificationChanged(false);

    return true;
}

bool DocumentManager::saveDocumentAs(const QString& newFileName)
{
    if (!m_fileManager) return false;

    m_currentFileName = newFileName;
    return saveDocument();
}

void DocumentManager::setContent(const QString& content)
{
    if (m_content != content) {
        m_content = content;
        m_isModified = true;
        emit modificationChanged(true);
    }
}

void DocumentManager::setModified(bool modified)
{
    if (m_isModified != modified) {
        m_isModified = modified;
        emit modificationChanged(modified);
    }
}


void DocumentManager::addCssToHtml(QString& html)
{
    QString css = R"(
    <style>
        pre, pre.sourceCode, div.sourceCode {
            overflow-x: auto;  /* ← Горизонтальная прокрутка */
            white-space: pre-wrap;  /* ← Перенос длинных строк */
            word-wrap: break-word;  /* ← Разрыв длинных слов */
            word-break: break-all;  /* ← Разрыв в любом месте */
            max-width: 100%;  /* ← Не шире экрана */
        }

        code {
            white-space: pre-wrap;  /* ← Перенос в code тоже */
            word-wrap: break-word;
        }

        /* Тёмная тема для блоков кода */
        pre, pre.sourceCode, div.sourceCode {
            background-color: #1e1e1e;
            padding: 15px;
            border-radius: 6px;
        }
    </style>
    )";

    int headEnd = html.indexOf("</head>");
    if (headEnd != -1) {
        html.insert(headEnd, css);
    } else {
        int bodyStart = html.indexOf("<body>");
        if (bodyStart != -1) {
            html.insert(bodyStart + 6, css);
        }
    }
}
