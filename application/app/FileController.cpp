#include "FileController.h"

#include <QFile>
#include <QDir>
#include <QUrl>
#include <QRegularExpression>

FileController::FileController(QObject *parent) : QObject(parent)
{

}

FileController::FileType_Em FileController::getFileType(const QString &filePath)
{
    QString _filePath = filePath;

    QString info = getUrlInfo(_filePath).toLocalFile();

    if (info.endsWith("pdf")) {
        return FileType_PDF;
    }

    return FileType_UNKNOWN;
}

QUrl FileController::getUrlInfo(QString filePath)
{
    QUrl url;
    // Just check if the path is an existing file.
    if (QFile::exists(filePath)) {
        url = QUrl::fromLocalFile(QDir::current().absoluteFilePath(filePath));
        return url;
    }

    const auto match = QRegularExpression(QStringLiteral(":(\\d+)(?::(\\d+))?:?$")).match(filePath);

    if (match.isValid()) {
        // cut away line/column specification from the path.
        filePath.chop(match.capturedLength());
    }

    // make relative paths absolute using the current working directory
    // prefer local file, if in doubt!
    url = QUrl::fromUserInput(filePath, QDir::currentPath(), QUrl::AssumeLocalFile);

    // in some cases, this will fail, e.g.
    // assume a local file and just convert it to an url.
    if (!url.isValid()) {
        // create absolute file path, we will e.g. pass this over dbus to other processes
        url = QUrl::fromLocalFile(QDir::current().absoluteFilePath(filePath));
    }
    return url;
}
