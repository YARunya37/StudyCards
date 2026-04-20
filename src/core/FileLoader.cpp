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
// qInfo(), qWarning() — вывод в консоль
#include <QCoreApplication> // Класс для доступа к информации о приложении
// Класс для доступа к информации о приложении
#include <QTemporaryDir>
#include <QTemporaryFile> // Классы для временных файлов
// Автоматическое создание и удаление временных файлов

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
    // Сначала ищем в папке приложения
    QString localPandoc = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/utils/pandoc/pandoc.exe");
    //QDir::cleanPath() — исправляет слеши (// -> /)
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
    return QFile::exists(pandocPath);//QFile::exists() — Проверяет, существует ли указанный путь к файлу.
    //Возвращает true если файл есть
}

// Конвертация исходного файла в HTML формат
bool ConvertToHtml(const QString& filePath, QString& outputPath, const QString& pandocPath)
{
    //Выводим отладочную информацию
    qInfo() << "ConvertToHtml: Starting...";
    qInfo() << "File:" << filePath;
    qInfo() << "Pandoc:" << pandocPath;
    //qInfo() — вывод в консоль

    //Проверяем что pandoc существует
    if (!isPandocAvailable(pandocPath)) {
        qWarning() << "ConvertToHtml: Pandoc not found:" << pandocPath;
        return false;//Если pandoc нет — выводим ошибку и выходим
    }

    // Генерируем путь для выходного файла
    outputPath = getOutputHtmlPath(filePath);

    // Создаём временную папку для картинок
    QString mediaDir = QDir::tempPath() + "/pandoc_media_" + //QDir::tempPath() — временная папка системы
                       QString::number(QDateTime::currentMSecsSinceEpoch());//currentMSecsSinceEpoch() — текущее время в миллисекундах
    QDir().mkpath(mediaDir);//mkpath() — создаёт папку

    //Используем QProcess вместо system() - для запуска pandoc
    QProcess process;
    process.setProgram(pandocPath); // Указываем какую программу запускать (pandoc.exe)
    process.setArguments({
        filePath,
        "-s",
        "-t", "html",
        "--wrap=none",
        "--embed-resources",
        "--standalone",
        "-o", outputPath
    });//Передаем аргументы командной строки pandoc

    qInfo() << "Running pandoc...";
    process.start();//Запускаем pandoc

    //Чтобы программа не зависла навсегда, ждём завершения (максимум 30 секунд)
    //Если дольше — убиваем процесс и выходим
    if (!process.waitForFinished(30000)) {
        qWarning() << "ConvertToHtml: Timeout -" << process.errorString();
        process.kill();
        return false;
    }

    // Проверяем код завершения
    if (process.exitCode() == 0) {
        qInfo() << "ConvertToHtml: Success!"; // exitCode() == 0 — успех
        return true;
    } else { // exitCode() != 0 — ошибка
        //fromLocal8Bit() — конвертируем в QString (для кириллицы)
        QString errorOutput = QString::fromLocal8Bit(process.readAllStandardError()); //readAllStandardError() — читаем сообщение об ошибке от pandoc
        qWarning() << "ConvertToHtml: Error (code" << process.exitCode() << "):";
        qWarning() << errorOutput;
        return false;
    }
}

// Загрузка документа (основная функция)
bool loadDocument(const QString& inputPath, QString& html, const QString& pandocPath)
{
    //Отладочный вывод:
    qInfo() << "=== FileLoader: loadDocument ===";
    qInfo() << "Input:" << inputPath;
    qInfo() << "Pandoc:" << pandocPath;

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

    // 5. Запускаем pandoc через QProcess
    QProcess process;//Создаёт объект для запуска внешней программы
    process.setProgram(pandocPath);// Указываем какую программу запускать
    process.setArguments({
        inputPath,
        "-f", "docx",
        "-t", "html",
        "--embed-resources",//Картинки в base64 внутри HTML
        "--standalone",//Добавляет <html>, <head>, <body>
        "-o", tempHtml.fileName()
    });//Передаём аргументы командной строки pandoc

    qInfo() << "Running pandoc...";
    process.start();

    // 6. Ждём завершения (30 секунд)
    if (!process.waitForFinished(30000)) {
        qWarning() << "FileLoader: Timeout -" << process.errorString();
        process.kill();
        return false;  // tempDir и tempHtml удалятся автоматически!
    }

    // 7. Проверяем код завершения pandoc
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

    QTextStream in(&file);//Создаём поток для чтения текста из файла
    html = in.readAll();// in.readAll() - Читает всё содержимое файла в одну строку
    file.close();

    qInfo() << "FileLoader: HTML loaded," << html.size() << "bytes";

    // 9. Добавляем CSS стили
    addCssStyles(html);

    qInfo() << "FileLoader: CSS added," << html.size() << "bytes";
    qInfo() << "=== FileLoader: Success ===";

    // tempDir и tempHtml удалятся сами когда функция завершится!

    return !html.isEmpty();
}
