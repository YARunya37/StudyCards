#ifndef FILEITEMSDIRECTORY_H
#define FILEITEMSDIRECTORY_H

#include <QDir>
#include <QMap>

class FileItemsDirectory
{
public:
    FileItemsDirectory(const QString& path_to_folder);

protected:
    // Текущая рабочая директория
    QDir currDir;
};

#endif // FILEITEMSDIRECTORY_H
