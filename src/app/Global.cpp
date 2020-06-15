#include "Global.h"

#include <QMimeType>
#include <QMimeDatabase>
#include <QDebug>

namespace Dr {
FileType fileType(const QString &filePath)
{
    FileType fileType = FileType::Unknown;

    const QMimeType mimeType = QMimeDatabase().mimeTypeForFile(filePath, QMimeDatabase::MatchContent);

    if (mimeType.name() == QLatin1String("application/pdf")) {
        fileType = PDF;
    } else if (mimeType.name() == QLatin1String("application/postscript")) {
        fileType = PS;
    } else if (mimeType.name() == QLatin1String("image/vnd.djvu")) {
        fileType = DjVu;
    } else if (mimeType.name() == QLatin1String("image/vnd.djvu+multipage")) {
        fileType = DjVu;
    } else if (mimeType.name() == QLatin1String("application/msword")) {
        fileType = DOC;
    } else if (mimeType.name() == QLatin1String("application/x-ole-storage")) {
        fileType = PPT_XLS;
    }

    return fileType;
}
}
