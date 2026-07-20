#ifndef DOCUMENTUI_H
#define DOCUMENTUI_H

#include <QObject>
#include <QString>
#include "filemanager.h"

class QAction;
class QTextEdit;
class FileTreeWidget;
class QMainWindow;

class DocumentUI : public QObject
{
    Q_OBJECT

public:
    explicit DocumentUI(
        QMainWindow* mainWindow,
        FileTreeWidget* fileTree,
        QTextEdit* textEditor,
        FileManager* fileManager,
        QObject* parent = nullptr);

    // Подключение действий меню
    void connectMenuActions(QAction* actionSave);

    // Для проверки возможности закрытия окна
    bool CanClose();

private slots:
    // Обработчик действия "сохранение"
    void onFileSave();

    // Обработчики сигналов
    void onFileDoubleClicked(const QString& fileName);
    void onTextChanged();
    void onModificationChanged(bool modified);

private:
    QMainWindow* m_mainWindow;
    FileTreeWidget* m_fileTree;
    QTextEdit* m_textEditor;
    FileManager* m_fileManager;
    // Флаг для игнорирования программных изменений
    bool m_isLoading;

    bool confirmSaveChanges();
    // Обновление заголовка окна
    void updateWindowTitle(const QString& fileName, bool isModified);
};

#endif // DOCUMENTUI_H
