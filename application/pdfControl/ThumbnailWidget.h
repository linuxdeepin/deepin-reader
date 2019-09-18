#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <DListWidget>
#include <DLabel>
#include <DPushButton>

#include <QPalette>
#include <QDebug>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QImage>
#include <QThread>
#include <QTimer>

#include "pdfControl/ThumbnailItemWidget.h"
#include "subjectObserver/CustomWidget.h"
#include "PagingWidget.h"
#include "docview/docummentproxy.h"

const int FIRST_LOAD_PAGES = 5;

class ThreadLoadImage : public QThread
{
public:
    ThreadLoadImage();

    void getPageImage(const int &page, QImage &);
    inline void setPages(const int pages)
    {
        m_pages = pages;
    }

    inline void clearImageMap()
    {
        m_imageMap.clear();
    }

    bool isLoaded()
    {
        return m_isLoaded;
    }

protected:
    virtual void run();

private:
    QMap<int, QImage> m_imageMap;
    int m_pages = -1; // 文件总页数
    bool m_isLoaded = false;// 是都加载完毕
};

/*
*缩略图列表页面
*/
class ThumbnailWidget : public CustomWidget
{
    Q_OBJECT

public:
    ThumbnailWidget(CustomWidget *parent = nullptr);

signals:
    void sigOpenFileOk();
    void sigSelectIndexPage(const int &);
    void sigJumpIndexPage(int);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;
    bool fillContantToList();

protected:
    void initWidget() override;

private:
    void setSelectItemBackColor(QListWidgetItem *);
    void setCurrentRow(const int &);
    void addThumbnailItem(const QImage &, const int &);

    inline int preRowVal() const
    {
        return m_preRow;
    }

    inline void setPreRowVal(const int &val)
    {
        m_preRow = val;
    }

    inline int totalPages() const
    {
        return m_totalPages;
    }

    inline void setTotalPages(const int &pages)
    {
        m_totalPages = pages;
    }

private slots:
    void slotShowSelectItem(QListWidgetItem *);
    void slotOpenFileOk();
    void slotJumpIndexPage(int);
    void loadThumbnailImage();

private:
    DListWidget *m_pThumbnailListWidget = nullptr;
    QVBoxLayout *m_pvBoxLayout = nullptr;
    DLabel *m_pThumbnailPageLabel = nullptr;
    PagingWidget *m_pPageWidget = nullptr;

    DLabel *m_pSonWidgetPageLabel = nullptr;
    DLabel *m_pSonWidgetContantLabel = nullptr;
    ThumbnailItemWidget *m_pThumbnailItemWidget = nullptr;

    int m_totalPages = -1; // 总页码数
    int m_preRow     = -1; // 前一次页码数
    ThreadLoadImage m_ThreadLoadImage;
    QTimer m_loadImageTimer;
};

#endif // THUMBNAILWIDGET_H
