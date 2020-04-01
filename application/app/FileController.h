#ifndef FILECONTROLLER_H
#define FILECONTROLLER_H

#include <QObject>

class FileController : public QObject
{
    Q_OBJECT
public:
    explicit FileController(QObject *parent = nullptr);

    enum FileType_Em {
        FileType_UNKNOWN = 0,
        FileType_PDF,
        FileType_DJVU,
        FileType_TIFF,
        FileType_PS,
        FileType_XPS
    };

    static FileType_Em getFileType(const QString &filePath);      //根据文件名，不做具体嗅探

    static QUrl getUrlInfo(QString filePath);

};

#endif // FILECONTROLLER_H
