#include "namedfileitem.h"

NamedFileItem::NamedFileItem(const QString& name)
    : name{name}
{}

QString NamedFileItem::Name() const
{
    return name;
}

void NamedFileItem::SetName(const QString &new_name)
{
    if(new_name != ""){
        this->name = new_name;
    }
}


