#pragma once

#include <QString>

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

// Поиск корня Obsidian vault
QString findObsidianVaultRoot(const QString& filePath);

// Поиск файла изображения
QString findImageFile(const QString& imagePath, const QString& mdFileDir);

// Нормализация Markdown (добавление пустых строк)
QString normalizeMarkdown(const QString& content);

// Конвертация Obsidian-синтаксиса в стандартный Markdown
QString convertObsidianToMarkdown(const QString& content, const QString& mdFilePath, const QString& tempDir);

// Загрузка документа (основная функция)
bool loadDocument(const QString& inputPath, QString& html, const QString& pandocPath, const QString& outputDir = "");

// Добавление CSS стилей к HTML
void addCssStyles(QString& htmlContent);
