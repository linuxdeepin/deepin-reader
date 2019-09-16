#include "publicfunc.h"
#include <QFile>
#include <uuid/uuid.h>


bool PublicFunc::copyFile(QFile &source, QFile &destination)
{
    const qint64 maxSize = 4096;
    qint64 size = -1;

    QScopedArrayPointer< char > buffer(new char[maxSize]);

    do
    {
        if((size = source.read(buffer.data(), maxSize)) < 0)
        {
            return false;
        }

        if(destination.write(buffer.data(), size) < 0)
        {
            return false;
        }
    }
    while(size > 0);

    return true;
}

QString PublicFunc::getUuid()
{
    uuid_t uuid;
    char str[36];
    uuid_generate(uuid);
    uuid_unparse(uuid, str);
    return  QString(str);
}

PublicFunc::PublicFunc()
{

}
