#ifndef COMMONSTRUCT_H
#define COMMONSTRUCT_H

#include <QList>
#include <QString>
#include <QDateTime>

#include <QMetaType>
#include <QPointF>
#include <QRectF>

enum ViewMode_EM {
    ViewMode_SinglePage = 0,
    ViewMode_FacingPage
};

enum RotateType_EM {
    RotateType_0 = 0,
    RotateType_90,
    RotateType_180,
    RotateType_270,
    RotateType_Normal,
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
enum AnnoteType_Em {
    Annote_Highlight = 0,
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

namespace Page {
enum LinkType_EM {
    LinkType_NULL = 0,
    LinkType_Goto,
    LinkType_GotoOtherFile,
    LinkType_Browse,
    LinkType_Execute
};

struct Link {
    QRectF boundary;
    int pageIndex;
    qreal left;
    qreal top;

    QString urlOrFileName;
    LinkType_EM type;

    Link() : boundary(), pageIndex(-1), left(qQNaN()), top(qQNaN()), urlOrFileName(), type(LinkType_NULL) {}
    Link(const QRectF &boundingRect, int index, qreal left = qQNaN(), qreal top = qQNaN()) : boundary(boundingRect), pageIndex(index), left(left), top(top), urlOrFileName(), type(LinkType_Goto) {}
    Link(const QRectF &boundingRect, const QString &url, LinkType_EM type = LinkType_Browse) : boundary(boundingRect), pageIndex(-1), left(qQNaN()), top(qQNaN()), urlOrFileName(url), type(type) {}
    Link(const QRectF &boundingRect, const QString &fileName, int index) : boundary(boundingRect), pageIndex(index), left(qQNaN()), top(qQNaN()), urlOrFileName(fileName), type(LinkType_GotoOtherFile) {}
};
}

struct Section;
typedef QVector< Section > Outline;
struct Section {
    QString title;
    Page::Link link;
    Outline children;
};
#endif // COMMONSTRUCT_H
