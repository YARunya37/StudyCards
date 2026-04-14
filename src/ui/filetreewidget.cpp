#include "filetreewidget.h"
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QDropEvent>
#include <QDragMoveEvent>
FileTreeWidget::FileTreeWidget(QWidget* parent) :
    QTreeWidget(parent)
{
    setUpTree();
}

void FileTreeWidget::AddFiles()
{
    // Вызываем диалоговое окно, в котором указываются файлы
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "Выбрать файлы",
        "",
        "Текстовые документы (*.docx *.md *.pdf)"
    );

    // Все указанные файлы конвертируем и добавляем в папку с файлами проекта
    foreach (auto filePath, files) {
        QString name = GetName(filePath).split(".")[0];

        // Если файла с таким именем ещё нет, то добавляем его в дерево
        if(!localFiles.contains(name)){
            // Путь к директории с файлами(внутри проекта) + имя данного файла с расширением
            QString dest = QCoreApplication::applicationDirPath() + "/resources/userfiles/"+ GetName(filePath);

            // ЗДЕСЬ ДОЛЖНА БЫТЬ КОНВЕРТАЦИЯ ФАЙЛОВ И ЗАПИСЬ ИХ В НУЖНУЮ ПАПКУ
            if(QFile::copy(filePath, dest)){
                // Добавляем в map локальный файл
                localFiles.insert(name, dest);

                // Создаем новую строчку в виджете с именем файла
                QTreeWidgetItem* new_item = new QTreeWidgetItem(this);
                new_item->setText(0, name);
            }
            // СДЕЛАТЬ УВЕДОМЛЕНИЕ ЧТО ФАЙЛЫ УЖЕ ДОБАЛЕНЫ
        }
    }
}


void FileTreeWidget::setUpTree()
{
    // Визуальная часть
    setHeaderLabel("Материалы");
    setMinimumWidth(50);
    setMaximumWidth(200);
    setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
    setColumnCount(1);

    // Восстанавливаем состояние на основе уже добавленных файлов
    restoreState();

    // Включаем возможность перемещать элементы внутри виджета
    this->setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
    setAcceptDrops(true);

    // test
    QTreeWidgetItem* new_item = new QTreeWidgetItem(this);
    new_item->setText(0, "PAPKA");
}

QString FileTreeWidget::GetName(QString file)
{
    QString name = "";
    // Циклом справа налево записываем символы в строку
    for (int i = file.length()-1; i >= 0; i--) {
        if(file[i] != "/" && file[i] != "\\"){
            name = file[i] + name;
        }
        else{
            // Если мы дошли до / или \, то имя записано полностью
            break;
        }
    }

    return name;
}

void FileTreeWidget::restoreState()
{
    // Путь к директориии с файлами, добавленными пользователем
    QString localfilesPath = QCoreApplication::applicationDirPath() + "/resources/userfiles/";
    // Получение всех файлов внури директории localfilesPath
    QStringList existing_files = QDir(localfilesPath).entryList(QDir::Files);

    // Каждый файл отображаем в виджете и заново добавляем в map локальных файлов
    foreach (auto file, existing_files) {
        QString name = GetName(file).split(".")[0];

        // Добавление в map
        localFiles.insert(name, localfilesPath + file);

        // Создаем новую строчку в виджете с именем файла
        QTreeWidgetItem* new_item = new QTreeWidgetItem(this);
        new_item->setText(0, name);
    }
}

void FileTreeWidget::dropEvent(QDropEvent *event)
{
    // Элемент над которым сейчас находится курсор
    auto targetItem = itemAt(event->position().toPoint());

    // Если бросаем элемент в пустое место, то вызываем стандартную реализацию
    if(!targetItem){
        QTreeWidget::dropEvent(event);
        return;
    }

    // Если просаем на предмет, пытаемся сделать его родителем
    if(dropIndicatorPosition() == QAbstractItemView::OnItem){

        // Если элемент, на который бросаем - файл, то игнорируем
        if(localFiles.contains(targetItem->text(0))){
            event->ignore();
            qInfo() << "Запрещено";
            return;
        }
        // Делаем файл дочерним к папке
        else{
            QTreeWidget::dropEvent(event);
            qInfo() << "Разрешено действие";
            return;
        }
    }
    // Вызов стандартной реализации на необработанные случаи
    QTreeWidget::dropEvent(event);

}

