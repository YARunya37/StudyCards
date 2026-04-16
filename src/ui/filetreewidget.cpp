#include "filetreewidget.h"
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QMenu>
FileTreeWidget::FileTreeWidget(QWidget* parent) :
    QTreeWidget(parent)
{
    setUpTree();

    // Для вызова кастомного контекстного меню
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeWidget::customContextMenuRequested, this, &FileTreeWidget::showContextMenu);

    // Измененение item по двойному щелчку
    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
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

    auto drop = dropIndicatorPosition();
    // Если просаем на предмет, пытаемся сделать его родителем
    if(drop == QAbstractItemView::OnItem ||
        drop == QAbstractItemView::AboveItem ||
        drop == QAbstractItemView::BelowItem){

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

void FileTreeWidget::showContextMenu(const QPoint &pos)
{
    QTreeWidgetItem* item = itemAt(pos);

    QMenu menu(this);
    // Если item nullptr, то это пустая область
    if(!item){
        // Создаём действие создания папки и подключаем его к реализации
        QAction* createAct = new QAction(QString("Создать папку"), this);
        connect(createAct, &QAction::triggered, this, &FileTreeWidget::createFolder);
        menu.addAction(createAct);
    }
    // Контекстное меню для элемента
    else{
        // Создаём действие удаления и подключаем его к реализации
        QAction* deleteAct = new QAction(QString("Удалить"), this);
        connect(deleteAct, &QAction::triggered, this, &FileTreeWidget::deleteItem);
        menu.addAction(deleteAct);
        // Если нажали по одной папке, то добавляем действие переименования
        if(!localFiles.contains(item->text(0)) && selectedItems().size() == 1){
            QAction* renameAct = new QAction(QString("Переименовать"), this);
            connect(renameAct, &QAction::triggered, this, &FileTreeWidget::renameItem);
            menu.addAction(renameAct);
        }
    }
    // Отображаем контекстное меню
    menu.exec(viewport()->mapToGlobal(pos));
}

void FileTreeWidget::createFolder()
{
    // Создаём item-папку
    QTreeWidgetItem* folder = new QTreeWidgetItem(this);
    folder->setText(0, QString("Новая папка"));
    // Деём возможность менять название папки
    folder->setFlags(folder->flags() | Qt::ItemIsEditable);
    addTopLevelItem(folder);

}

void FileTreeWidget::deleteItem()
{
    // Перебираем все выбранные элементы на удаление
    foreach(auto item, selectedItems()){
        // Удаляем детей
        deleteChildren(item);
        // Удаляем сам элемент
        // Если файл, нужно удалить его по пути и из map
        if(localFiles.contains(item->text(0))){
            QFile::remove(localFiles.value(item->text(0)));
            localFiles.remove(item->text(0));
        }
        delete item;
    }
}

void FileTreeWidget::renameItem(){
    auto items = selectedItems();
    editItem(items.value(0), 0);
}

void FileTreeWidget::deleteChildren(QTreeWidgetItem *folder)
{
    // Идём справа налево, чтобы при уменьшении списка дочерних элемента, не потерять никого
    //(иначе нужен отступ назад при удалении, т.к. индексы сместятся на один влево после удалённого)
    for(int i = folder->childCount()-1; i >= 0; i--) {
        auto currChild = folder->child(i);
        // Если дочерний элемент - папка, то удаляем все файлы уже в ней
        if(!localFiles.contains(currChild->text(0))){
            deleteChildren(currChild);
            delete currChild;
        }
        // Удалаяем файл из проекта, массива, дерева
        else{
            qInfo() << "Удаляем файл по пути:" << localFiles.value(currChild->text(0));
            QFile::remove(localFiles.value(currChild->text(0)));
            localFiles.remove(currChild->text(0));
            delete currChild;
        }
    }
}
