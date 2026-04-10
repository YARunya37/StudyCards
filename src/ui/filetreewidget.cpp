#include "filetreewidget.h"
#include <QFileDialog>
#include <QDebug>
#include <QFileSystemModel>
FileTreeWidget::FileTreeWidget(QWidget* parent) :
    QTreeWidget(parent)
{
    setUpTree();
}

void FileTreeWidget::AddFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "Выбрать файлы",
        "",
        "Текстовые документы (*.docx *.md *.pdf)"
    );
    foreach (auto filePath, files) {
        QString name = GetName(filePath).split(".")[0];
        // Если файла с таким именем ещё нет, то добавляем его в дерево
        if(!localFiles.contains(name)){
            localFiles.insert(name, filePath);
            QTreeWidgetItem* new_item = new QTreeWidgetItem(this);
            new_item->setText(0, name);
        }
    }
}

void FileTreeWidget::setUpTree()
{
    setHeaderLabel("Материалы");
    setMinimumWidth(50);
    setMaximumWidth(200);
    setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
    setColumnCount(1);
}

QString FileTreeWidget::GetName(QString file)
{
    QString name = "";
    for (int i = file.length()-1; i >= 0; i--) {
        if(file[i] != "/" && file[i] != "\\"){
            name = file[i] + name;
        }
        else{
            break;
        }
    }
    return name;
}

