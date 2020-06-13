#include "publicfunc.h"
#include <QFile>
#include <QUuid>

bool PublicFunc::copyFile(QFile &source, QFile &destination)
{
    const qint64 maxSize = 4096;
    qint64 size = -1;

    QScopedArrayPointer< char > buffer(new char[maxSize]);

    do {
        if ((size = source.read(buffer.data(), maxSize)) < 0) {
            return false;
        }

        if (destination.write(buffer.data(), size) < 0) {
            return false;
        }
    } while (size > 0);

    return true;
}

QString PublicFunc::getUuid()
{
    const QString &uuid = QUuid::createUuid().toString();
    return  uuid.mid(1, uuid.length() - 2);
}

PublicFunc::PublicFunc()
{

}
