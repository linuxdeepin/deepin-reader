#ifndef COMMONSTRUCT_H
#define COMMONSTRUCT_H

#include <QList>
#include <QString>
#include <QDateTime>

#include <QMetaType>

typedef struct SEARCH_RESULT {
    unsigned int ipage;
    QList<QString> listtext;
    SEARCH_RESULT()
    {
        ipage = 0;
        listtext.clear();
    }

} stSearchRes;

Q_DECLARE_METATYPE(stSearchRes)

typedef struct HIGHLIGHT_CONTENTS {
    unsigned int ipage;
    QString strcontents;
    QString struuid;
    HIGHLIGHT_CONTENTS()
    {
        ipage = 0;
        strcontents.clear();
        struuid.clear();
    }

} stHighlightContent;

typedef struct FILE_INFO {
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
    FILE_INFO()
    {
        iWidth = iHeight = iNumpages = 0;
        boptimization = bsafe = false;
        size = 0.0;
    }


} stFileInfo;

typedef struct ICONANNOTATION {
    QString uuid;
    QPointF position;
    QString strnote;

    ICONANNOTATION()
    {
        uuid.clear();
        position = QPointF(0.0, 0.0);
        strnote.clear();
    }

} stIconAnnotation;

#endif // COMMONSTRUCT_H
