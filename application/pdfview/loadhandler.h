#ifndef LOADHANDLER_H
#define LOADHANDLER_H

#include <QString>

#include "model.h"
class ModelDocument;
class LoadHandler
{
public:
     static LoadHandler* instance();
    ~LoadHandler();
    enum FileType
    {
        Unknown = 0,
        PDF,
        PS,
        DjVu,
        Image,
        GZip,
        BZip2,
        XZ,
        ZIP,
        EPUB,
        XPS,
        FB2,
        CBZ
    };

    static QLatin1String fileTypeName(FileType fileType);
    static QStringList openFilter();
    ModelDocument* loadDocument(const QString& filePath);
private:
    Q_DISABLE_COPY(LoadHandler)
    static LoadHandler* s_instance;
     QMap< FileType, Plugin* > m_plugins;
    LoadHandler();
};

#endif // LOADHANDLER_H
