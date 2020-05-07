#ifndef PUBLICFUNC_H
#define PUBLICFUNC_H

const double EPSINON = 0.00000001;

class QFile;
class QString;
class PublicFunc
{
public:
    static bool copyFile(QFile &source, QFile &destination);
    static QString getUuid();
private:
    PublicFunc();
};

#endif // PUBLICFUNC_H
