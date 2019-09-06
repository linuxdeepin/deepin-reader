#ifndef PUBLICFUNCTION_H
#define PUBLICFUNCTION_H

#include <QString>

namespace PF {
static QString getQrcPath(const QString &imageName, const QString &state)
{
    return QString(":/resources/image/%1/%2.svg").arg(state).arg(imageName);
}

}

#endif // PUBLICFUNCTION_H
