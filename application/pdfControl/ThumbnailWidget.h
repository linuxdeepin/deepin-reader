#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <DLabel>
#include <DPushButton>

#include <QPalette>
#include <QDebug>
#include <QVBoxLayout>
#include <QThread>
#include <QTimer>

#include "pdfControl/ThumbnailItemWidget.h"
#include "subjectObserver/CustomWidget.h"
#include "PagingWidget.h"
#include "docview/docummentproxy.h"
#include "CustomListWidget.h"
//const int FIRST_LOAD_PAGES = 20;

/**
 * @brief The ThreadLoadImage class
 * @brief   加载缩略图线程
 */

class ThumbnailWidget;

class ThreadLoadImage : public QThread
{
    Q_OBJECT
public:
    ThreadLoadImage(QObject *parent = nullptr);

public:
    void stopThreadRun();

    inline void setPages(const int pages)
    {
        m_pages = pages;
    }

    inline int endPage()
    {
        return m_nEndPage;
    }

    inline void setIsLoaded(const bool &load)
    {
        m_isLoaded = load;
    }

    inline bool isLoaded()
    {
        return m_isLoaded;
    }

    inline void setThumbnail(ThumbnailWidget *thumbnail)
    {
        m_pThumbnailWidget = thumbnail;
    }

    inline void setStartAndEndIndex()
    {
        m_nStartPage = 0;  // 加载图片起始页码
        m_nEndPage = 19;   // 加载图片结束页码
    }

signals:
    void signal_loadImage(const int &, const QImage &);

protected:
    void run() Q_DECL_OVERRIDE;

private:
    int m_pages = 0; // 文件总页数
    bool m_isLoaded = false;// 是都加载完毕
    ThumbnailWidget *m_pThumbnailWidget = nullptr;
    int m_nStartPage = 0;  // 加载图片起始页码
    int m_nEndPage = 19;   // 加载图片结束页码
};

/*
*缩略图列表页面
*/
class ThumbnailWidget : public CustomWidget
{
    Q_OBJECT

public:
    ThumbnailWidget(CustomWidget *parent = nullptr);
    ~ThumbnailWidget() Q_DECL_OVERRIDE;

signals:
    void sigOpenFileOk();
    void sigCloseFile();

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void fillContantToList();

    inline bool isLoading()
    {
        return m_isLoading;
    }

protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    void setSelectItemBackColor(QListWidgetItem *);
    void addThumbnailItem(const int &);

private slots:
    void slotOpenFileOk();
    void slotLoadThumbnailImage();
    void slotFileViewToListPage(const int &);
    void slotCloseFile();

private:
    CustomListWidget *m_pThumbnailListWidget = nullptr;         // list widget item子界面
    PagingWidget *m_pPageWidget = nullptr;                      // 跳页界面
    ThumbnailItemWidget *m_pOldThumbnailItemWidget = nullptr;   // 当前选中的项
    int m_totalPages = -1;                                      // 总页码数
    ThreadLoadImage m_ThreadLoadImage;                          // 加载缩略图线程
    bool m_isLoading = false;                                   // 缩略图list是否初始化完毕
//    QTimer m_loadImageTimer;
};

#endif // THUMBNAILWIDGET_H
