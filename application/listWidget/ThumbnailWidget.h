#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <DWidget>
#include <DListWidget>
#include <QListWidgetItem>
#include <DLabel>
#include <QVBoxLayout>
#include <DPushButton>
#include <QDebug>

#include <listWidget/ThumbnailItemWidget.h>
#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"
#include "header/MsgHeader.h"
#include "PagingWidget.h"

DWIDGET_USE_NAMESPACE

/*
*缩略图列表页面
*/
class ThumbnailWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT

public:
    ThumbnailWidget(DWidget *parent = nullptr);
    ~ThumbnailWidget() override;

public:
    // IObserver interface
    int update(const int &, const QString &) override;

private:
    void initWidget();
    void setSelectItemBackColor(QListWidgetItem *);
    void setCurrentRow(const int &);

signals:
    void sigSelectIndexPage(const int &);

public slots:
    void slotSetJumpToPage(const int &);

private slots:
    void slotShowSelectItem(QListWidgetItem *);

private:
    DListWidget *m_pThumbnailListWidget = nullptr;
    QVBoxLayout *m_pvBoxLayout = nullptr;
    DLabel *m_pThumbnailPageLabel = nullptr;
    PagingWidget *m_pPageWidget = nullptr;

    DWidget *m_itemWidget = nullptr;
    DLabel *m_pSonWidgetPageLabel = nullptr;
    ThumbnailItemWidget *m_pThumbnailItemWidget = nullptr;

    MsgSubject    *m_pMsgSubject = nullptr;
};

#endif // THUMBNAILWIDGET_H
