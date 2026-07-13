#ifndef GROUP_H
#define GROUP_H

#include <QString>
#include "studycard.h"
#include <QMap>
#include "diritemsmanager.h"
#include "namedfileitem.h"
#include <QStringList>
// Класс, отвечающий за конкретную группу пользователя
class Group : public DirItemsManager, public NamedFileItem
{
public:
    Group(const QString& name);
    // Метод для получения указателя на билет по имени
    StudyCardWidget* GetCard(const QString& name) const;
    // Метод для удаления всех билетов в группе
    void Clear();
    // Метод для получения имён всех билетов
    QStringList GetAllCards() const;
public slots:
    bool CreateCard(const QString& question_text);
    bool CreateCard(StudyCardWidget* card);
    bool DeleteCard(const QString& card_name);
    void RenameCard(const QString& old_name, const QString& new_name);
private:
    // Список билетов
    QMap<QString, StudyCardWidget*> cards;
    // Путь к группе
    const QString group_path;
    // Генерация уникального ID для билета (C1, C2, C3...)
    QString generateCardId();
    // Миграция старых билетов (где имя папки = тексту вопроса)
    void MigrateOldCards();
};

#endif // GROUP_H
