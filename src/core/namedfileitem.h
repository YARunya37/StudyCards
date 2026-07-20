#ifndef NAMEDFILEITEM_H
#define NAMEDFILEITEM_H

#include <QString>

class NamedFileItem
{
public:
    NamedFileItem(const QString& name);

    QString Name() const;
    virtual void SetName(const QString& new_name);
protected:
    QString name;
};

#endif // NAMEDFILEITEM_H
