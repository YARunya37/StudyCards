#pragma once

#include <QString>

using namespace std;

// Получение полного пути к pandoc
QString getPandocPath();

// Получение пути выходного файла с расширением .html
QString getOutputHtmlPath(const QString& inputPath);

// Получение расширения входящего файла
QString getFileExtension(const QString& filePath);

// Проверка поддерживаемого формата
bool isSupportedFormat(const QString& filePath);

// Проверка доступности pandoc
bool isPandocAvailable(const QString& pandocPath);

// Конвертация исходного файла в HTML
bool ConvertToHtml(const QString& filePath, QString& outputPath, const QString& pandocPath);

// Загрузка документа (основная функция)
bool loadDocument(const QString& inputPath, QString& html, const QString& pandocPath);

// Добавление CSS стилей к HTML
void addCssStyles(QString& htmlContent);
