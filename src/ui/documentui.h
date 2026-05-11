#ifndef DOCUMENTUI_H
#define DOCUMENTUI_H

#include <QObject>
#include <QString>

class QAction;
class QTextEdit;
class FileTreeWidget;
class DocumentManager;
class QMainWindow;

class DocumentUI : public QObject
{
    Q_OBJECT

public:
    explicit DocumentUI(
        QMainWindow* mainWindow,
        FileTreeWidget* fileTree,
        QTextEdit* textEditor,
        DocumentManager* docManager,
        QObject* parent = nullptr);

    // Подключение действий меню
    void connectMenuActions(QAction* actionSave);

private slots:
    // Обработчик действия "сохранение"
    void onFileSave();

    // Обработчики сигналов
    void onFileDoubleClicked(const QString& fileName);
    void onTextChanged();
    void onContentLoaded(const QString& content);
    void onModificationChanged(bool modified);

private:
    QMainWindow* m_mainWindow;
    FileTreeWidget* m_fileTree;
    QTextEdit* m_textEditor;
    DocumentManager* m_docManager;
    bool m_isLoading;

    QString extractBodyContent(const QString& html);
    bool confirmSaveChanges();
    // Обновление заголовка окна
    void updateWindowTitle(const QString& fileName, bool isModified);
};

#endif // DOCUMENTUI_H
