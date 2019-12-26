#ifndef DOCUMMENTFILEHELPER_H
#define DOCUMMENTFILEHELPER_H

#include <QObject>

#include "docview/docummentproxy.h"
#include "subjectObserver/IObserver.h"

#include "controller/NotifySubject.h"

/**
 * @brief The DocummentFileHelper class
 * 封装 DocView 函数调用
 */

class DocummentFileHelper : public QObject, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(DocummentFileHelper)

private:
    explicit DocummentFileHelper(QObject *parent = nullptr);

public:
    static DocummentFileHelper *instance()
    {
        static DocummentFileHelper subject;
        return &subject;
    }

    ~DocummentFileHelper() Q_DECL_OVERRIDE;

public:
    void setDocProxy(DocummentProxy *);

signals:
    void sigDealWithData(const int &, const QString &);
    void sigDealWithKeyMsg(const QString &);
    void sigFileSlider(const int &);

    void sigFileCtrlContent();

public:
    bool save(const QString &filepath, bool withChanges);
    bool closeFile();
    void docBasicInfo(stFileInfo &info);
    void setViewFocus();

public:
    bool mouseSelectText(const QPoint &start, const QPoint &stop);
    bool mouseBeOverText(const QPoint &point);
    Page::Link *mouseBeOverLink(const QPoint &point);
    void mouseSelectTextClear();
    bool getSelectTextString(QString &st);

public:
    void onClickPageLink(Page::Link *pLink);
    QPoint global2RelativePoint(const QPoint &globalpoint);
    bool pageMove(const double &mvx, const double &mvy);
    int  pointInWhichPage(const QPoint &pos);
    int  getPageSNum();
    int currentPageNo();
    bool pageJump(const int &pagenum);
    bool getImage(const int &pagenum, QImage &image, const double &width, const double &height);

private:
    QImage roundImage(const QPixmap &img_in, const int &radius);

public:
    bool showMagnifier(const QPoint &point);
    bool closeMagnifier();

    bool setBookMarkState(const int &page, const bool &state);

public:
    void setScaleRotateViewModeAndShow(double scale, RotateType_EM rotate, ViewMode_EM viewmode);
//    bool setViewModeAndShow(const ViewMode_EM &viewmode);
//    void scaleRotateAndShow(const double &scale, const RotateType_EM &rotate);

public:
    double adaptWidthAndShow(const double &width);
    double adaptHeightAndShow(const double &height);

public:
    void clearsearch();

public:
    void getAllAnnotation(QList<stHighlightContent> &listres);
    QString addAnnotation(const QPoint &startpos, const QPoint &endpos, const QColor &color = Qt::yellow);
    void changeAnnotationColor(const int &ipage, const QString &uuid, const QColor &color);
    bool annotationClicked(const QPoint &pos, QString &strtext, QString &struuid);
    void removeAnnotation(const QString &struuid, const int &ipage = -1);
    QString removeAnnotation(const QPoint &pos);
    void setAnnotationText(const int &ipage, const QString &struuid, const QString &strtext);
    void getAnnotationText(const QString &struuid, QString &strtext, const int &ipage);
    void jumpToHighLight(const QString &uuid, const int &ipage);

public:
    void search(const QString &strtext, QMap<int, stSearchRes> &resmap, const QColor &color);
    void findNext();
    void findPrev();

public:
    bool getAutoPlaySlideStatu();
    void setAutoPlaySlide(const bool &autoplay, const int &timemsec = 3000);

private:
    void setAppShowTitle(const QString &);
    void initConnections();
    void onOpenFile(const QString &filePaths);
    void onOpenFiles(const QString &filePaths);

    void onSaveFile();
    void onSaveAsFile();

    // IObserver interface
    void sendMsg(const int &msgType, const QString &msgContent = "") Q_DECL_OVERRIDE;
    void notifyMsg(const int &msgType, const QString &msgContent = "") Q_DECL_OVERRIDE;

private slots:
    void slotDealWithData(const int &, const QString &);
    void slotDealWithKeyMsg(const QString &);

    void slotCopySelectContent(const QString &);
    void slotFileSlider(const int &);
    void slotFileCtrlContent();

private:
    DocummentProxy          *m_pDocummentProxy = nullptr;   //  文档操作代理类

    SubjectThread           *m_pNotifySubject = nullptr;

    QList<int>              m_pMsgList;
    QList<QString>          m_pKeyMsgList;
    QString                 m_szFilePath = "";
    DocType_EM              m_nCurDocType = DocType_NULL;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // DOCUMMENTFILEHELPER_H
