#include "FileLoader.h"

#include <QProcess> // Класс для запуска внешних программ
// Запускаем pandoc.exe из нашего кода
#include <QFile> // Класс для работы с файлами
// Чтение/запись файлов
#include <QTextStream> // Класс для чтения/записи текста
// Читаем HTML файл в строку
#include <QDir> // Класс для работы с директориями
// Создание папок, очистка путей
#include <QFileInfo> //  Класс для получения информации о файле
// Получаем расширение, имя файла без расширения
#include <QStandardPaths> // Класс для стандартных путей системы
// Получаем путь к временной папке (C:/Users/.../Temp)
#include <QDateTime> // Класс для работы с датой и временем
// Создаём уникальные имена для временных файлов
#include <QDebug> // Класс для отладочного вывода

#include <QCoreApplication> // Класс для доступа к информации о приложении
// Класс для доступа к информации о приложении
#include <QTemporaryDir>
#include <QTemporaryFile> // Классы для временных файлов
// Автоматическое создание и удаление временных файлов

#include <QRegularExpression>
#include <QDirIterator>
//Храним стили которые добавим в HTML

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
    //Если есть <head> — вставляем CSS внутрь, если нет — в начало
    if (htmlContent.contains("<head>", Qt::CaseInsensitive)) { // Проверяем есть ли тег <head> в HTML
        // Qt::CaseInsensitive — не важно <head> или <HEAD>
        htmlContent.replace("<head>", "<head>\n" + CSS_STYLES, Qt::CaseInsensitive);//Заменяем <head> на (<head> + наши стили)
    }
    else
    {
        htmlContent.prepend(CSS_STYLES); //prepend() — вставить в начало строки
    }
}

// Получение полного пути к pandoc
QString getPandocPath()
{
    QString localPandoc = QDir::cleanPath(
        QCoreApplication::applicationDirPath() + "/utils/pandoc/pandoc"
    );

    // Добавляем .exe для Windows
#ifdef Q_OS_WIN
    localPandoc += ".exe";
#endif

    if (QFile::exists(localPandoc)) return localPandoc;
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
    //suffix() — получение расширения (например "DOCX")
    //toLower() — переводим в нижний регистр ("docx")
}

// Проверка поддерживаемого формата
bool isSupportedFormat(const QString& filePath)
{
    QString ext = getFileExtension(filePath); //Получаем расширение файла
    return (ext == "docx" || ext == "doc" || ext == "md" || ext == "markdown");//Проверяем поддерживаемый формат
    //Возвращает: true если один из форматов, false если нет
}
// Проверяем существует ли файл pandoc
bool isPandocAvailable(const QString& pandocPath)
{
    return QFile::exists(pandocPath);
    //Возвращает true если файл есть
}

QString findObsidianVaultRoot(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();

    qInfo() << "Searching for .obsidian vault root from:" << dir.absolutePath();

    // Идём вверх по дереву папок
    while (true) {
        // Проверяем есть ли .obsidian в текущей папке
        if (QFile::exists(dir.absoluteFilePath(".obsidian"))) {
            qInfo() << "Found .obsidian at:" << dir.absolutePath();
            return dir.absolutePath();
        }

        // Поднимаемся на уровень выше
        if (!dir.cdUp()) {
            break;  // Достигли корня диска
        }
    }

    // Не нашли .obsidian — возвращаем папку файла
    qWarning() << "⚠ .obsidian not found, using file directory";
    return QFileInfo(filePath).absoluteDir().absolutePath();
}

QString findImageFile(const QString& imagePath, const QString& mdFilePath)
{
    if (imagePath.isEmpty()) return "";

        // Нормализуем путь и проверяем что он внутри vault
        QString cleanImage = QDir::cleanPath(imagePath);
        if (cleanImage.startsWith("..") || cleanImage.startsWith("/")) {
            qWarning() << "Blocked path traversal attempt:" << imagePath;
            return "";
        }

    qInfo() << "Searching for image:" << imagePath;

    // 1. Абсолютный путь
    if (QFile::exists(imagePath)) {
        qInfo() << "Found (absolute):" << imagePath;
        return imagePath;
    }

    // 2. Находим корень Obsidian vault
    QString vaultRoot = findObsidianVaultRoot(mdFilePath);
    QString mdFileDir = QFileInfo(mdFilePath).absolutePath();

    // 3. Ищем относительно папки MD файла
    QString relativePath = QDir(mdFileDir).filePath(imagePath);
    if (QFile::exists(relativePath)) {
        qInfo() << "Found (relative to MD):" << relativePath;
        return relativePath;
    }

    // 4. Ищем в подпапках vault (относительно корня)
    QStringList searchDirs = {
        "_attachments",
        "assets",
        "media",
        "images",
        "img",
        "files",
        "resources"
    };

    for (const QString& subDir : searchDirs) {
        QString fullPath = QDir(vaultRoot).filePath(subDir + "/" + imagePath);
        if (QFile::exists(fullPath)) {
            qInfo() << "Found in vault/" << subDir << ":" << fullPath;
            return fullPath;
        }
    }

    // 5. Ищем в подпапках относительно MD файла
    for (const QString& subDir : searchDirs) {
        QString fullPath = QDir(mdFileDir).filePath(subDir + "/" + imagePath);
        if (QFile::exists(fullPath)) {
            qInfo() << "Found in MD folder/" << subDir << ":" << fullPath;
            return fullPath;
        }
    }

    // 6. Рекурсивный поиск по vault (медленно, но надёжно)
    if (imagePath.contains("Pasted image", Qt::CaseInsensitive)) {
        qInfo() << "Recursive search in vault...";
        QString baseName = QFileInfo(imagePath).baseName();

        QDirIterator it(vaultRoot,
                       {"*.png", "*.jpg", "*.jpeg", "*.gif", "*.webp"},
                       QDir::Files,
                       QDirIterator::Subdirectories);

        while (it.hasNext()) {
            QString file = it.next();
            if (QFileInfo(file).baseName() == baseName) {
                qInfo() << "Found similar:" << file;
                return file;
            }
        }
    }

    qWarning() << "Image not found:" << imagePath;
    return "";
}

QString normalizeMarkdown(const QString& content)
{
    QString result = content;

    qInfo() << "=== normalizeMarkdown START ===";
    qInfo() << "Original size:" << content.size() << "bytes";

    // 1. Перед заголовками (##, ###, ####)
    result.replace(QRegularExpression("([^\n])\n(#{1,6} )"), "\n\n\\2");

    // 2. Перед списками (1., -, *)
    result.replace(QRegularExpression("([^\n])\n(\\d+\\. |- |\\* )"), "\n\n\\2");

    // 3. Перед изображениями ![]()
    result.replace(QRegularExpression("([^\n])\n(!\\[.*?\\]\\(.*?\\))"), "\n\n\\2");

    // 4. Перед таблицами (| ...)
    result.replace(QRegularExpression("([^\n])\n(\\|.+)"), "\n\n\\2");

    // 5. Перед кодом (```)
    result.replace(QRegularExpression("([^\n])\n(```)"), "\n\n\\2");

    // После кода (```)
    result.replace(QRegularExpression("(```)\n([^\n])"), "\\1\n\n\\2");

    // 6. Перед цитатами (>)
    result.replace(QRegularExpression("([^\n])\n(>)"), "\n\n\\2");

    // 7. После горизонтальных линий (---)
    result.replace(QRegularExpression("(---)\n([^\n])"), "\\1\n\n\\2");

    // Убираем множественные пустые строки (более 2 подряд)
    result.replace(QRegularExpression("\n{3,}"), "\n\n");

    qInfo() << "Normalized size:" << result.size() << "bytes";
        qInfo() << "First 300 chars:";
        qInfo() << result.left(300);
        qInfo() << "=== normalizeMarkdown END ===";
    return result;
}

// Конвертация Obsidian-синтаксиса в стандартный Markdown
QString convertObsidianToMarkdown(const QString& content,
                                   const QString& mdFilePath,
                                   const QString& tempDir)
{
    QString result = content;
    QString vaultRoot = findObsidianVaultRoot(mdFilePath);

    qInfo() << "=== Converting Obsidian syntax ===";
    qInfo() << "MD file:" << mdFilePath;
    qInfo() << "Vault root:" << vaultRoot;
    qInfo() << "Temp dir:" << tempDir;

    QRegularExpression obsidianImage(R"(!\[\[([^\]\|]+)(?:\|([^\]]+))?\]\])");
    QRegularExpressionMatchIterator it = obsidianImage.globalMatch(result);

    QList<QPair<int, int>> imageReplacements;
    QList<QString> imageNewValues;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString imageName = match.captured(1).trimmed();
        QString altText = match.captured(2).trimmed();

        qInfo() << "Processing image:" << imageName;

        // Ищем файл
        QString foundPath = findImageFile(imageName, mdFilePath);

        QString replacement;
        if (foundPath.isEmpty()) {
            replacement = QString("*(Не найдено: %1)*").arg(imageName);
        } else {
            // Копируем во временную папку
            QString destPath = QDir(tempDir).filePath(imageName);
            QFileInfo fileInfo(imageName);

            if (fileInfo.dir().path() != ".") {
                QDir(tempDir).mkpath(fileInfo.dir().path());
                destPath = QDir(tempDir).filePath(imageName);
            }

            if (QFile::copy(foundPath, destPath)) {
                qInfo() << "Copied to:" << destPath;

                // ← ← ← Создаём ПРАВИЛЬНЫЙ синтаксис с абсолютным путём!
                if (altText.isEmpty()) {
                    replacement = QString("![](%1)").arg(destPath);
                } else {
                    replacement = QString("![%1](%2)").arg(altText, destPath);
                }
            } else {
                replacement = QString("*(Ошибка копирования)*");
            }
        }

        imageReplacements.append(qMakePair(match.capturedStart(), match.capturedLength()));
        imageNewValues.append(replacement);
    }

    // Применяем замены
    for (int i = imageReplacements.size() - 1; i >= 0; --i) {
        result.replace(imageReplacements[i].first, imageReplacements[i].second, imageNewValues[i]);
    }

    // Wiki-ссылки
    result.replace(QRegularExpression(R"(\[\[([^\]|]+)\|([^\]]+)\]\])"), R"([\2](\1.md))");
    result.replace(QRegularExpression(R"(\[\[([^\]]+)\]\])"), R"([\1](\1.md))");

    qInfo() << "=== Conversion complete ===";
    return result;
}
// Загрузка документа (основная функция)
bool loadDocument(const QString& inputPath, QString& html, const QString& pandocPath, const QString& outputDir)
{
    //Отладочный вывод:
    qInfo() << "FileLoader: Loading" << inputPath;

    // 1. Проверка входного файла
    if (!QFile::exists(inputPath)) {
        qWarning() << "FileLoader: Input file not found:" << inputPath;
        return false;
    }

    // 2. Проверяем формат файла
    if (!isSupportedFormat(inputPath)) {
        qWarning() << "FileLoader: Unsupported format:" << getFileExtension(inputPath);
        return false;
    }
    // 3. Проверка pandoc; Если pandocPath пустой — получаем его автоматически
    if (!isPandocAvailable(pandocPath)) {
        qWarning() << "FileLoader: Pandoc not found:" << pandocPath;
        return false;
    }

    // 4. Временные файлы (автоматическая очистка)
    QTemporaryDir tempDir; // Создаёт объект для временной папки
    QTemporaryFile tempHtml; // Создаёт объект для временного файла

    //Проверка что папка создалась
    if (!tempDir.isValid()) {
        qWarning() << "FileLoader: Failed to create temp dir";
        return false;
    }

    //Открываем временный файл для записи
    if (!tempHtml.open()) { // open() — создаёт файл на диске и открывает его
        qWarning() << "FileLoader: Failed to create temp file";
        return false;
    }
    tempHtml.close(); //Закрываем файл, чтобы pandoc мог в него записать результат

    //Отладочный вывод путей. Чтобы видеть в логах куда pandoc пишет результат
    qInfo() << "Temp dir:" << tempDir.path(); //tempDir.path() — полный путь к временной папке
    qInfo() << "Temp file:" << tempHtml.fileName();//tempHtml.fileName() — полный путь к временному файлу

    // 5. Для MD файлов: читаем и конвертируем Obsidian-синтаксис
        QString ext = getFileExtension(inputPath);
        bool isMarkdown = (ext == "md" || ext == "markdown");

        QString markdownContent;
        if (isMarkdown) {
            QFile inputFile(inputPath);
            if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&inputFile);
                markdownContent = in.readAll();
                inputFile.close();

                markdownContent = normalizeMarkdown(markdownContent);
                qInfo() << "Normalized Markdown formatting";

                // Конвертируем ![[...]] → !(...)
                markdownContent = convertObsidianToMarkdown(markdownContent, inputPath, tempDir.path());

                qInfo() << "Converted Obsidian syntax to standard Markdown";
            }
        }
    // 6. Запускаем pandoc через QProcess
    QProcess process;//Создаёт объект для запуска внешней программы
    process.setProgram(pandocPath);// Указываем какую программу запускать
    if (isMarkdown && !markdownContent.isEmpty()) {
            process.setArguments({
                "-f", "markdown+pipe_tables+grid_tables-yaml_metadata_block-smart",
                "-t", "html",
                "--embed-resources",
                "--standalone",
                "-o", tempHtml.fileName()
            });
            process.start();
            process.write(markdownContent.toUtf8());  // Передаём конвертированный текст!
            process.closeWriteChannel();
        } else {
            // ← ← ← DOCX: передаём путь к файлу
            process.setArguments({
                inputPath,
                "-t", "html",
                "--embed-resources",
                "--standalone",
                "-o", tempHtml.fileName()
            });
            process.start();
        }

    // 7. Ждём завершения (30 секунд)
    if (!process.waitForFinished(30000)) {
        qWarning() << "FileLoader: Timeout -" << process.errorString();
        process.kill();
        return false;  // tempDir и tempHtml удалятся автоматически!
    }

    // 8. Проверяем код завершения pandoc
    if (process.exitCode() != 0) {
        QString error = QString::fromLocal8Bit(process.readAllStandardError());
        qWarning() << "FileLoader: Error (code" << process.exitCode() << "):" << error;
        return false;  // tempDir и tempHtml удалятся автоматически!
    }

    // 9. Читаем HTML из временного файла
    QFile file(tempHtml.fileName());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "FileLoader: Cannot read temp file:" << tempHtml.fileName();
        return false;
    }

    QTextStream in(&file);//Создаём поток для чтения текста из файла
    html = in.readAll();// in.readAll() - Читает всё содержимое файла в одну строку
    file.close();

    qInfo() << "FileLoader: HTML loaded," << html.size() << "bytes";

    if (!outputDir.isEmpty()) {
        // Создаём ПОЛНЫЙ путь к файлу (папка + имя файла)
        QFileInfo inputInfo(inputPath);
        QString fileName = inputInfo.baseName() + ".html";  // ← Lab_7.html
        QString fullPath = outputDir + "/" + fileName;      // ← output/Lab_7.html

        qInfo() << "FileLoader: Saving to:" << fullPath;

        // Создаём папку если нет
        QDir().mkpath(outputDir);  // Создаём ПАПКУ

        // Сохраняем файл
        QFile outFile(fullPath);  // ПОЛНЫЙ путь!
        if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&outFile);
            out << html;
            outFile.close();
            qInfo() << "FileLoader: HTML saved:" << fullPath;
        } else {
            qWarning() << "FileLoader: Failed to save:" << fullPath;
        }
    }
    qInfo() << "First 500 chars of HTML:";
    qInfo() << html.left(500);
    // 11. Добавляем CSS стили
    addCssStyles(html);

    qInfo() << "FileLoader: CSS added," << html.size() << "bytes";
    qInfo() << "=== FileLoader: Success ===";

    // tempDir и tempHtml удалятся сами когда функция завершится!

    return !html.isEmpty();
}
