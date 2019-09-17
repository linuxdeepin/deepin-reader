#ifndef DOCUMMENTHELPER_H
#define DOCUMMENTHELPER_H

#include <QObject>

#include "docview/docummentproxy.h"
#include "subjectObserver/IObserver.h"

/**
 * @brief The DocummentHelper class
 * @brief   文档处理中间类，　用于处理文件数据
 */


class DocummentHelper : public QObject, public IObserver
{
    Q_OBJECT

private:
    explicit DocummentHelper(QObject *parent = nullptr);

public:
    static DocummentHelper *instance()
    {
        static DocummentHelper helper;
        return &helper;
    }

private slots:
    void slotDocFilePageChanged(int);

public:
    bool openFile(const DocType_EM &type, const QString &filepath);
    bool save(const QString &filepath, const bool &withChanges);
    void search(const QString &strtext, QMap<int, stSearchRes> &resmap, const QColor &color);
    void clearsearch();
    void docBasicInfo(stFileInfo &info);

    //  获取指定页，指定大小的图片
    bool getImage(const int &pagenum, QImage &image, const int &width, const int &height);
    //  坐标转换
    QPoint global2RelativePoint(const QPoint &globalpoint);

    //  放大镜操作
    bool showMagnifier(const QPoint &globalpoint);
    bool closeMagnifier();

    //  页面移动、切换
    int currentPageNo() const;
    bool pageJump(const int &pagenum);
    int  getPageSNum();
    bool pageMove(const double &x, const double &y);

    //  文字选择
    bool mouseSelectText(const QPoint &start, const QPoint &end);
    bool mouseBeOverText(const QPoint &);
    void mouseSelectTextClear();

    //  幻灯片操作
    bool showSlideModel();

    //  缩放操作
    void scaleRotateAndShow(const double &scale, const RotateType_EM &rotate);
    bool setViewModeAndShow(const ViewMode_EM &viewmode);


private:
    DocummentProxy          *m_pDocummentProxy = nullptr;       //  渠成　提供的文档处理类

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    void sendMsg(const int &, const QString &) override;

private:
    void setObserverName(const QString &name) override;
};

#endif // DOCUMMENTHELPER_H
