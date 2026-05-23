#ifndef NAMEDFILEITEM_H
#define NAMEDFILEITEM_H

#include <QString>

class NamedFileItem
{
public:
    NamedFileItem(const QString& name);

    QString Name() const;
    void SetName(const QString& new_name);
protected:
    QString name;
};

#endif // NAMEDFILEITEM_H
