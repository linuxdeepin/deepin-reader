#ifndef COMMONSTRUCT_H
#define COMMONSTRUCT_H

#include <QList>
#include <QString>
#include <QDateTime>

#include <QMetaType>

enum ViewMode_EM {
    ViewMode_SinglePage = 0,
    ViewMode_FacingPage
};

enum DocType_EM {
    DocType_NULL = 0,
    DocType_PDF,
    DocType_DJVU,
    DocType_TIFF,
    DocType_PS,
    DocType_XPS

};

enum RotateType_EM {
    RotateType_Normal = 0,
    RotateType_0,
    RotateType_90,
    RotateType_180,
    RotateType_270
};

//The type of the icon for this text annotation.
enum TextAnnoteType_Em {
    TextAnnoteType_Note = 0,
    TextAnnoteType_Comment,
    TextAnnoteType_Help,
    TextAnnoteType_Insert,
    TextAnnoteType_Key,
    TextAnnoteType_NewParagraph,
    TextAnnoteType_Paragraph,
};

//note type
enum AnnoteType_Em{
    Annote_Highlight=0,
    Annote_Text,
};

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

struct stWord {
    QString s;
    QRectF rect;
};

#endif // COMMONSTRUCT_H
