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

#include "pdfControl/ThumbnailItemWidget.h"
#include "subjectObserver/CustomWidget.h"
#include "PagingWidget.h"


/*
*缩略图列表页面
*/
class ThumbnailWidget : public CustomWidget
{
    Q_OBJECT

public:
    ThumbnailWidget(CustomWidget *parent = nullptr);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;

protected:
    void initWidget() override;

private:
    void setSelectItemBackColor(QListWidgetItem *);
    void setCurrentRow(const int &);
    void addThumbnailItem(const QImage &, const int &);
    void fillContantToList();

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

signals:
    void sigSelectIndexPage(const int &);

private slots:
    void slotShowSelectItem(QListWidgetItem *);

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
};

#endif // THUMBNAILWIDGET_H
