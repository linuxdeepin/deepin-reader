#ifndef PUBLICFUNC_H
#define PUBLICFUNC_H

class QFile;
class PublicFunc
{
public:
    static bool copyFile(QFile& source, QFile& destination);
private:
    PublicFunc();
};

#endif // PUBLICFUNC_H
