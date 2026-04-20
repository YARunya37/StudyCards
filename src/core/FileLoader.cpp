#include "FileLoader.h"

#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QTemporaryFile>

//CSS СТИЛИ
static const QString CSS_STYLES =
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

//Добавление CSS к HTML
void addCssStyles(QString& htmlContent)
{
    if (htmlContent.contains("<head>", Qt::CaseInsensitive)) {
        htmlContent.replace("<head>", "<head>\n" + CSS_STYLES, Qt::CaseInsensitive);
    }
    else
    {
        htmlContent.prepend(CSS_STYLES);
    }
}

// Получение полного пути к pandoc
QString getPandocPath()
{
    // Сначала ищем в папке приложения
    QString localPandoc = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/utils/pandoc/pandoc.exe");
    if (QFile::exists(localPandoc)) return localPandoc;

    // Если нет, ищем в системном PATH (удобно для разработки)
    return QStandardPaths::findExecutable("pandoc");
}

// Получение пути выходного файла с расширением .html
QString getOutputHtmlPath(const QString& inputPath)
{
    QFileInfo fileInfo(inputPath);
    return fileInfo.absolutePath() + "/" + fileInfo.baseName() + ".html";
}

// Получение расширения входящего файла
QString getFileExtension(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.suffix().toLower();
}

// Проверка поддерживаемого формата
bool isSupportedFormat(const QString& filePath)
{
    QString ext = getFileExtension(filePath);
    return (ext == "docx" || ext == "doc" || ext == "md" || ext == "markdown");
}
// Проверка доступности pandoc
bool isPandocAvailable(const QString& pandocPath)
{
    return QFile::exists(pandocPath);
}

// Конвертация исходного файла в HTML формат
bool ConvertToHtml(const QString& filePath, QString& outputPath, const QString& pandocPath)
{
    qInfo() << "ConvertToHtml: Starting...";
    qInfo() << "File:" << filePath;
    qInfo() << "Pandoc:" << pandocPath;

    if (!isPandocAvailable(pandocPath)) {
        qWarning() << "ConvertToHtml: Pandoc not found:" << pandocPath;
        return false;
    }

    // Генерируем путь для выходного файла
    outputPath = getOutputHtmlPath(filePath);

    // Создаём временную папку для извлечения медиа (картинок)
    QString mediaDir = QDir::tempPath() + "/pandoc_media_" +
                       QString::number(QDateTime::currentMSecsSinceEpoch());
    QDir().mkpath(mediaDir);

    //Используем QProcess вместо system()
    QProcess process;
    process.setProgram(pandocPath);
    process.setArguments({
        filePath,
        "-s",
        "-t", "html",
        "--wrap=none",
        "--embed-resources",
        "--standalone",
        "-o", outputPath
    });

    qInfo() << "Running pandoc...";
    process.start();

    // Ждём завершения (30 секунд)
    if (!process.waitForFinished(30000)) {
        qWarning() << "ConvertToHtml: Timeout -" << process.errorString();
        process.kill();
        return false;
    }

    // Проверяем результат
    if (process.exitCode() == 0) {
        qInfo() << "ConvertToHtml: Success!";
        return true;
    } else {
        QString errorOutput = QString::fromLocal8Bit(process.readAllStandardError());
        qWarning() << "ConvertToHtml: Error (code" << process.exitCode() << "):";
        qWarning() << errorOutput;
        return false;
    }
}

// Загрузка документа (основная функция)
bool loadDocument(const QString& inputPath, QString& html, const QString& pandocPath)
{
    qInfo() << "=== FileLoader: loadDocument ===";
    qInfo() << "Input:" << inputPath;
    qInfo() << "Pandoc:" << pandocPath;

    // 1. Проверка входного файла
    if (!QFile::exists(inputPath)) {
        qWarning() << "FileLoader: Input file not found:" << inputPath;
        return false;
    }

    // 2. Проверка формата
    if (!isSupportedFormat(inputPath)) {
        qWarning() << "FileLoader: Unsupported format:" << getFileExtension(inputPath);
        return false;
    }

    // 3. Проверка pandoc
    if (!isPandocAvailable(pandocPath)) {
        qWarning() << "FileLoader: Pandoc not found:" << pandocPath;
        return false;
    }

    // 4. Временные файлы (автоматическая очистка)
    QTemporaryDir tempDir;
    QTemporaryFile tempHtml;

    if (!tempDir.isValid()) {
        qWarning() << "FileLoader: Failed to create temp dir";
        return false;
    }

    if (!tempHtml.open()) {
        qWarning() << "FileLoader: Failed to create temp file";
        return false;
    }
    tempHtml.close();

    qInfo() << "Temp dir:" << tempDir.path();
    qInfo() << "Temp file:" << tempHtml.fileName();

    // 5. Запускаем pandoc через QProcess
    QProcess process;
    process.setProgram(pandocPath);
    process.setArguments({
        inputPath,
        "-f", "docx",
        "-t", "html",
        "--embed-resources",
        "--standalone",
        "-o", tempHtml.fileName()
    });

    qInfo() << "Running pandoc...";
    process.start();

    // 6. Ждём завершения (30 секунд)
    if (!process.waitForFinished(30000)) {
        qWarning() << "FileLoader: Timeout -" << process.errorString();
        process.kill();
        return false;  // tempDir и tempHtml удалятся автоматически!
    }

    // 7. Проверяем результат
    if (process.exitCode() != 0) {
        QString error = QString::fromLocal8Bit(process.readAllStandardError());
        qWarning() << "FileLoader: Error (code" << process.exitCode() << "):" << error;
        return false;  // tempDir и tempHtml удалятся автоматически!
    }

    // 8. Читаем HTML из временного файла
    QFile file(tempHtml.fileName());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "FileLoader: Cannot read temp file:" << tempHtml.fileName();
        return false;
    }

    QTextStream in(&file);
    html = in.readAll();
    file.close();

    qInfo() << "FileLoader: HTML loaded," << html.size() << "bytes";

    // 9. Добавляем CSS стили
    addCssStyles(html);

    qInfo() << "FileLoader: CSS added," << html.size() << "bytes";
    qInfo() << "=== FileLoader: Success ===";

    // tempDir и tempHtml удалятся сами когда функция завершится!

    return !html.isEmpty();
}
