#ifndef DOCUMMENTFILEHELPER_H
#define DOCUMMENTFILEHELPER_H

#include <QObject>

#include "docview/docummentproxy.h"


/**
 * @brief The DocummentFileHelper class
 * 封装 DocView 函数调用
 */

class DocummentFileHelper : public QObject
{
    Q_OBJECT
private:
    explicit DocummentFileHelper(QObject *parent = nullptr);

public:
    static DocummentFileHelper *instance()
    {
        static DocummentFileHelper subject;
        return &subject;
    }

public:
    bool save(const QString &filepath, bool withChanges);
    bool closeFile();
    void docBasicInfo(stFileInfo &info);

public:
    bool mouseSelectText(QPoint start, QPoint stop);
    bool mouseBeOverText(QPoint point);
    Page::Link *mouseBeOverLink(QPoint point);
    void mouseSelectTextClear();
    bool getSelectTextString(QString &st);

public:
    void onClickPageLink(Page::Link *pLink);
    QPoint global2RelativePoint(QPoint globalpoint);
    bool pageMove(double mvx, double mvy);
    int  pointInWhichPage(QPoint pos);
    int  getPageSNum();
    int currentPageNo();
    bool pageJump(int pagenum);
    bool getImage(int pagenum, QImage &image, double width, double height);

public:
    bool showMagnifier(QPoint point);
    bool closeMagnifier();

    bool setBookMarkState(int page, bool state);

public:
    bool setViewModeAndShow(ViewMode_EM viewmode);
    void scaleRotateAndShow(double scale, RotateType_EM rotate);

public:
    double adaptWidthAndShow(const double &width);
    double adaptHeightAndShow(const double &height);

public:
    void clearsearch();

public:
    void getAllAnnotation(QList<stHighlightContent> &listres);
    QString addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color = Qt::yellow);
    bool annotationClicked(const QPoint &pos, QString &strtext, QString &struuid);
    void removeAnnotation(const QString &struuid, int ipage = -1);
    QString removeAnnotation(const QPoint &pos);
    void setAnnotationText(int ipage, const QString &struuid, const QString &strtext);
    void getAnnotationText(const QString &struuid, QString &strtext, int ipage);
    void jumpToHighLight(const QString &uuid, int ipage);

public:
    void search(const QString &strtext, QMap<int, stSearchRes> &resmap, const QColor &color);
    void findNext();
    void findPrev();

private:
    void setAppShowTitle(const QString &);
    void initConnections();
    QString getFileFilter();
    QString getFilePath(const QString &inputPath);

    void setCurDocuType(const QString &);

    void sendMsg(const int &msgType, const QString &msgContent = "");

private slots:
    void slotOpenFile(const QString &filePaths);
    void slotSaveFile();
    void slotSaveAsFile();
    void slotCopySelectContent(const QString &);
    void slotFileSlider(const int &);

private:
    DocummentProxy          *m_pDocummentProxy = nullptr;   //  文档操作代理类

    QString                 m_szFilePath = "";
    DocType_EM              m_nCurDocType = DocType_NULL;
};

#endif // DOCUMMENTFILEHELPER_H
