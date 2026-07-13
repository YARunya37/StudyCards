#include "group.h"
#include <QCoreApplication>
#include <QDir>
#include <QRegularExpression>

Group::Group(const QString& name)
    : NamedFileItem(name),
    group_path{"/resources/usergroups/" + name},
    DirItemsManager(QCoreApplication::applicationDirPath() + "/resources/usergroups/" + name)
{
    MigrateOldCards();

    foreach(auto item, RestoreItems()){
        auto card = new StudyCardWidget(nullptr, group_path, item);
        cards.insert(item, card);
    }
}

QString Group::generateCardId()
{
    // Сканируем существующие папки и находим максимальный ID
    QDir dir(QCoreApplication::applicationDirPath() + group_path);
    QStringList folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    int max_id = 0;
    QRegularExpression re("^C(\\d+)$");

    foreach (const QString& folder, folders) {
        QRegularExpressionMatch match = re.match(folder);
        if (match.hasMatch()) {
            int id = match.captured(1).toInt();
            if (id > max_id)
                max_id = id;
        }
    }

    return "C" + QString::number(max_id + 1);
}

void Group::MigrateOldCards()
{
    QDir dir(QCoreApplication::applicationDirPath() + group_path);
    QStringList folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    QRegularExpression re("^C(\\d+)$");

    foreach (const QString& folder, folders) {
        // Если папка НЕ соответствует формату C1, C2...
        if (!re.match(folder).hasMatch()) {
            // Проверяем, есть ли header.html (значит это старый билет)
            QString folder_path = dir.absolutePath() + "/" + folder;
            QString header_path = folder_path + "/header.html";

            if (QFile::exists(header_path)) {
                // Генерируем новый ID для этой папки
                QString new_id = generateCardId();

                // Переименовываем папку
                if (dir.rename(folder, new_id)) {
                    qInfo() << "Миграция билета:" << folder << "->" << new_id;
                } else {
                    qWarning() << "Не удалось переименовать папку:" << folder;
                }
            }
        }
    }
}

StudyCardWidget* Group::GetCard(const QString &name) const
{
    if(cards.keys().contains(name))
        return cards.value(name);
    else
        return nullptr;
}

void Group::Clear()
{
    foreach (auto item, cards.keys()) {
        DeleteItem(item);
    }
}

QStringList Group::GetAllCards() const
{
    return cards.keys();
}

bool Group::CreateCard(const QString &question_text)
{
    // Генерируем уникальный ID вместо использования текста вопроса
    QString card_id = generateCardId();

    if(AddItem(card_id)){
        // Создаём виджет с ID папки
        StudyCardWidget* card = new StudyCardWidget(nullptr, group_path, card_id);
        cards.insert(card_id, card);

        // Устанавливаем текст вопроса (это запишет его в header.html)
        card->SetName(question_text);

        return true;
    }
    else{
        qInfo() << "Ошибка при создании билета";
        return false;
    }
}

bool Group::CreateCard(StudyCardWidget* card)
{
    if(AddItem(card->Name())){
        StudyCardWidget* local_card = card;
        card->setParent(nullptr);
        cards.insert(local_card->Name(), local_card);
        return true;
    }
    else{
        qInfo() << "Ошибка при создании билета";
        return false;
    }
}

bool Group::DeleteCard(const QString &card_name)
{
    if(DeleteItem(card_name)){
        auto card = GetCard(card_name);

        // Проверка на nullptr
        if(!card)
            return false;

        delete card;
        cards.remove(card_name);

        return true;
    }
    else{
        qInfo() << "Ошибка при удалении билета";
        return false;
    }
}

void Group::RenameCard(const QString &old_name, const QString &new_name)
{
    if(RenameItem(old_name, new_name)){
        auto card = GetCard(old_name);

        // Проверка на nullptr
        if(!card)
            return;

        // Удаление группы
        cards.remove(old_name);

        // Вставка с новым именем
        cards.insert(new_name, card);
        // Установка нового имени группе
        card->SetName(new_name);
    }
}
