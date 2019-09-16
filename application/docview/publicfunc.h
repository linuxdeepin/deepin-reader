#ifndef PUBLICFUNC_H
#define PUBLICFUNC_H

class QFile;
class QString;
class PublicFunc
{
public:
    static bool copyFile(QFile& source, QFile& destination);
    static QString getUuid();
private:
    PublicFunc();
};

#endif // PUBLICFUNC_H
