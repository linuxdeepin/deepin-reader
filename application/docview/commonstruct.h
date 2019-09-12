#ifndef COMMONSTRUCT_H
#define COMMONSTRUCT_H
#include <QList>
#include <QString>
#include <QDateTime>

typedef struct SEARCH_RESULT{
    unsigned int ipages;
    QList<QString> listtext;
    SEARCH_RESULT(){
        ipages=0;
        listtext.clear();
    }

}stSearchRes;

typedef struct FILE_INFO{
    QString strFilepath;
    QString strTheme;
    QString strAuther;
    QString strKeyword;
    QString strProducter;
    QString strCreater;
    QDateTime CreateTime;
    QDateTime ChangeTime;
    QString strFormat;
    unsigned int iWidth;
    unsigned int iHeight;
    unsigned int iNumpages;
    bool     boptimization;
    bool     bsafe;
    float    size;
    FILE_INFO(){
        iWidth=iHeight=iNumpages=0;
        boptimization=bsafe=false;
        size=0.0;
    }


}stFileInfo;

#endif // COMMONSTRUCT_H
