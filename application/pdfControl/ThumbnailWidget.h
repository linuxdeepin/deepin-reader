#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <DLabel>
#include <DPushButton>

#include <QPalette>
#include <QDebug>
#include <QVBoxLayout>
#include <QImage>
#include <QThread>
#include <QTimer>

#include "pdfControl/ThumbnailItemWidget.h"
#include "subjectObserver/CustomWidget.h"
#include "PagingWidget.h"
#include "docview/docummentproxy.h"
#include "CustomListWidget.h"
//const int FIRST_LOAD_PAGES = 20;

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
    void signal_loadImage(int, QImage);
protected:
    void run() override;

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
    ~ThumbnailWidget() override;

signals:
    void sigOpenFileOk();
    void sigCloseFile();

private slots:
    void slot_loadImage(int, QImage);
public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;
    void fillContantToList();

    inline bool isLoading()
    {
        return m_isLoading;
    }

protected:
    void initWidget() override;

private:
    void setSelectItemBackColor(QListWidgetItem *);
    void addThumbnailItem(const int &);

private slots:
    void slotOpenFileOk();
    void slotLoadThumbnailImage();
    void slotFileViewToListPage(const int &);
    void slotCloseFile();

private:
    CustomListWidget *m_pThumbnailListWidget = nullptr;
    PagingWidget *m_pPageWidget = nullptr;

    ThumbnailItemWidget *m_pOldThumbnailItemWidget = nullptr;   //  当前选中的项

    int m_totalPages = -1; // 总页码数
    ThreadLoadImage m_ThreadLoadImage;
    bool m_isLoading = false;
//    QTimer m_loadImageTimer;
};

#endif // THUMBNAILWIDGET_H
