#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <DWidget>
#include <DListWidget>
#include <QListWidgetItem>
#include <DLabel>
#include <QVBoxLayout>
#include <DPushButton>
#include <QDebug>


#include "PagingWidget.h"

DWIDGET_USE_NAMESPACE

/*
*缩略图列表页面
*/
class ThumbnailWidget : public DWidget
{
    Q_OBJECT

public:
    ThumbnailWidget(DWidget *parent = nullptr);
    ~ThumbnailWidget();

private:
    void initWidget();
    void setSelectItemBackColor(QListWidgetItem *);

signals:
    void selectIndexPage(const int &);

public slots:
    void onSetJumpToPage(const int&);

private slots:
    void onShowSelectItem(QListWidgetItem *);

private:
    DListWidget * m_pThumbnailListWidget = nullptr;
    QVBoxLayout * m_pvBoxLayout = nullptr;
    DLabel * m_pThumbnailPageLabel = nullptr;
    PagingWidget * m_pPageWidget = nullptr;

    DWidget * m_itemWidget = nullptr;
    DLabel * m_pSonWidgetPageLabel = nullptr;
};

#endif // THUMBNAILWIDGET_H
