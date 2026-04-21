#include "filetreewidget.h"
#include <QFileDialog>
#include <QDebug>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
FileTreeWidget::FileTreeWidget(QWidget* parent) :
    QTreeWidget(parent)
{
    setUpTree();

    // Для вызова кастомного контекстного меню
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeWidget::customContextMenuRequested, this, &FileTreeWidget::showContextMenu);

    // Измененение item только по двойному щелчку
    setEditTriggers(NoEditTriggers);
    connect(this, &QTreeWidget::itemDoubleClicked, this, &FileTreeWidget::renameItem);

    // Создание filemanager
    fmn = new FileManager(this);

    // Восстанавливаем состояние на основе уже добавленных файлов
    restoreState();
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


    // Отображаем в дереве добавленные файлы
    auto added_files = fmn->add_files(files);
    if(!added_files.isEmpty()){
        foreach(auto file, added_files){
            QTreeWidgetItem* new_item = new QTreeWidgetItem(this);
            new_item->setText(0, file);
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

    // Включаем возможность перемещать элементы внутри виджета
    this->setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
    setAcceptDrops(true);
}


void FileTreeWidget::restoreState()
{
    QStringList added_files;

    // Восстановление папок

    QStringList folders = fmn->get_existing_folders();
    foreach(auto folder, folders){
        QTreeWidgetItem* parent_folder = nullptr;

        // Поиск элемента по тексту
        QList<QTreeWidgetItem*> parent_found = this->findItems(
            folder,
            Qt::MatchExactly | Qt::MatchRecursive, // Флаги поиска
            0  // Номер колонки
            );

        // Если нашли папку, то сохраняем указатель на неё
        if(!parent_found.isEmpty()){
            parent_folder = parent_found.first();
        }
        // Создаём её в корне
        else{
            parent_folder = new QTreeWidgetItem(this);
            parent_folder->setText(0, folder);
        }

        foreach (auto child, fmn->get_children(folder)) {
            QTreeWidgetItem* childItem = nullptr;

            // Поиск элемента по тексту
            QList<QTreeWidgetItem*> child_found = this->findItems(
                child,
                Qt::MatchExactly | Qt::MatchRecursive, // Флаги поиска
                0  // Номер колонки
                );

            // Если нашли в дереве, то сохраняем указатель
            if(!child_found.isEmpty()){
                childItem = child_found.first();
                // Просто удаляем от старого родителя
                if(childItem->parent()){
                    childItem->parent()->removeChild(childItem);
                }
                else{
                    // Если на верхнем уровне дерева
                    int index = indexOfTopLevelItem(childItem);
                    takeTopLevelItem(index);
                }

                // Добавляем к новому
                parent_folder->addChild(childItem);
            }
            // Если нет, то создаём
            else{
                childItem = new QTreeWidgetItem(parent_folder);
                childItem->setText(0, child);
            }
            added_files.append(childItem->text(0));
        }
    }

    // Проверяем все файлы
    foreach (auto file, fmn->get_existing_files()) {
        // Если файл с таким именем ещё не добавили, то делаем это
        if(!added_files.contains(file)){
            QTreeWidgetItem* new_item = new QTreeWidgetItem(this);
            new_item->setText(0, file);
        }
    }
}

void FileTreeWidget::dropEvent(QDropEvent *event)
{
    // Элемент над которым сейчас находится курсор
    auto targetItem = itemAt(event->position().toPoint());
    auto draggedItem = selectedItems().first();
    // Если бросаем элемент в пустое место, то вызываем стандартную реализацию
    if(!targetItem){
        // Добавляем в файловую систему
        // Если файл находился в какой-то папке
        if(draggedItem->parent()){
            fmn->add_item_to_folder(draggedItem->text(0), "", draggedItem->parent()->text(0));
        }
        else{
            fmn->add_item_to_folder(draggedItem->text(0), "");
        }
        QTreeWidget::dropEvent(event);
        return;
    }

    auto drop = dropIndicatorPosition();
    // Если просаем на предмет, пытаемся сделать его родителем
    if(drop == QAbstractItemView::OnItem ||
        drop == QAbstractItemView::AboveItem ||
        drop == QAbstractItemView::BelowItem){

        // Если элемент, на который бросаем - файл, то игнорируем
        if(fmn->is_file(targetItem->text(0))){
            event->ignore();
            qInfo() << "Запрещено";
            return;
        }
        // Делаем файл дочерним к папке
        else{
            // Добавляем в файловую систему

            // Если бросаем на элемет, то папкой будет он
            if(drop == QAbstractItemView::OnItem){
                // Если файл находился в какой-то папке
                if(draggedItem->parent()){
                    fmn->add_item_to_folder(draggedItem->text(0), targetItem->text(0), draggedItem->parent()->text(0));
                }
                else{
                    fmn->add_item_to_folder(draggedItem->text(0), targetItem->text(0));
                }
            }
            // Если выше или ниже, то родитель - родитель элемента, над/под которым бросили
            else{
                if(targetItem->parent()){
                    // Если файл находился в какой-то папке
                    if(draggedItem->parent()){
                        fmn->add_item_to_folder(draggedItem->text(0), targetItem->parent()->text(0), draggedItem->parent()->text(0));
                    }
                    else{
                        fmn->add_item_to_folder(draggedItem->text(0), targetItem->parent()->text(0));
                    }
                }
            }

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
        // Если нажали по одной папке, то добавляем действие переименования и создания папки
        if(!fmn->is_file(item->text(0)) && selectedItems().size() == 1){

            QAction* createAct = new QAction(QString("Создать папку"), this);
            connect(createAct, &QAction::triggered, this, &FileTreeWidget::createFolder);
            menu.addAction(createAct);

            QAction* renameAct = new QAction(QString("Переименовать"), this);
            connect(renameAct, &QAction::triggered, this, &FileTreeWidget::renameItem);
            menu.addAction(renameAct);
        }
        // Создаём действие удаления и подключаем его к реализации
        QAction* deleteAct = new QAction(QString("Удалить"), this);
        connect(deleteAct, &QAction::triggered, this, &FileTreeWidget::deleteItem);
        menu.addAction(deleteAct);
    }
    // Отображаем контекстное меню
    menu.exec(viewport()->mapToGlobal(pos));
}

void FileTreeWidget::createFolder()
{
    auto selected = selectedItems();
    // Если создаём в папке
    if(selected.size() == 1 && !fmn->is_file(selected.value(0)->text(0))){
        // Создаём item-папку под выбранной папкой
        QTreeWidgetItem* folder = new QTreeWidgetItem(selected.value(0));
        folder->setText(0, QString("Новая папка"));
        // Деём возможность менять название папки
        folder->setFlags(folder->flags() | Qt::ItemIsEditable);
        addTopLevelItem(folder);
        // Предлагаем сразу изменить название папки
        bool isOk;
        QString new_name = QInputDialog::getText(
            this,
            "Создать папку",
            "Название папки:",
            QLineEdit::Normal,
            "Новая папка",
            &isOk
            );
        if(isOk && !new_name.trimmed().isEmpty()){
            folder->setText(0, new_name);
        }
        // Добавляем файл в систему
        fmn->add_folder(folder->text(0));
        fmn->add_item_to_folder(folder->text(0), selected.value(0)->text(0));
    }
    else{
        // Создаём item-папку в корне дерева
        QTreeWidgetItem* folder = new QTreeWidgetItem(this);
        folder->setText(0, QString("Новая папка"));
        // Деём возможность менять название папки
        folder->setFlags(folder->flags() | Qt::ItemIsEditable);
        addTopLevelItem(folder);
        // Предлагаем сразу изменить название папки
        bool isOk;
        QString new_name = QInputDialog::getText(
            this,
            "Создать папку",
            "Название папки:",
            QLineEdit::Normal,
            "Новая папка",
            &isOk
            );
        if(isOk && !new_name.trimmed().isEmpty()){
            folder->setText(0, new_name);
        }
        // Добавляем файл в систему
        fmn->add_folder(folder->text(0));
    }
}

void FileTreeWidget::deleteItem()
{
    // Перебираем все выбранные элементы на удаление
    foreach(auto item, selectedItems()){
        // Удаляем детей
        deleteChildren(item);
        // Удаляем сам элемент
        try {
            // Если это файл в корне проекта, то сразу удаляем его
            if(fmn->is_file(item->text(0)) && !item->parent()){
                fmn->remove_file(item->text(0));
            }
            else if(item->parent()){
                fmn->remove_item_from_folder(item->text(0), item->parent()->text(0));
            }
            else{
                fmn->remove_item_from_folder(item->text(0), "");
            }
            delete item;
        } catch (const std::runtime_error& e) {
            qCritical() << "Ошибка:" << e.what();
            // Обработка ошибки
            QMessageBox::critical(nullptr, "Ошибка", e.what());
        }
    }
}

// Возвращает старое имя
QString FileTreeWidget::renameItem(){
    auto items = selectedItems();
    QString old_name = items.at(0)->text(0);

    // Если пытаемся изменить имя файла, то ничего не делаем
    if(fmn->is_file(old_name)){
        return old_name;
    }

    bool isOk;
    QString new_name = QInputDialog::getText(
        this,
        "Изменить название папки",
        "Название папки:",
        QLineEdit::Normal,
        old_name,
        &isOk
    );

    // Если отмененно или имя пустое
    if(!isOk || new_name.trimmed().isEmpty()){
        return old_name;
    }
    // Записываем новое имя
    items.at(0)->setText(0, new_name);

    // Переименуем папку в файлах
    fmn->rename_folder(old_name, items.at(0)->text(0));

    return old_name;
}

void FileTreeWidget::deleteChildren(QTreeWidgetItem *folder)
{
    // Идём справа налево, чтобы при уменьшении списка дочерних элемента, не потерять никого
    //(иначе нужен отступ назад при удалении, т.к. индексы сместятся на один влево после удалённого)
    for(int i = folder->childCount()-1; i >= 0; i--) {
        auto currChild = folder->child(i);
        // Если дочерний элемент - папка, то удаляем все файлы уже в ней
        if(!fmn->is_file(currChild->text(0))){
            deleteChildren(currChild);
            delete currChild;
        }
        // Удалаяем файл из проекта, дерева
        else{
            delete currChild;
        }
    }
}
