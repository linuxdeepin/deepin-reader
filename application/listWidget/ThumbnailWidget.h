#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <DListWidget>
#include <QListWidgetItem>
#include <DLabel>
#include <QVBoxLayout>
#include <DPushButton>
#include <QDebug>

#include "listWidget/ThumbnailItemWidget.h"
#include "header/CustomWidget.h"
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

private slots:
    void slotShowSelectItem(QListWidgetItem *);

private:
    DListWidget *m_pThumbnailListWidget = nullptr;
    QVBoxLayout *m_pvBoxLayout = nullptr;
    DLabel *m_pThumbnailPageLabel = nullptr;
    PagingWidget *m_pPageWidget = nullptr;

    //DWidget *m_itemWidget = nullptr;
    DLabel *m_pSonWidgetPageLabel = nullptr;
    DLabel *m_pSonWidgetContantLabel = nullptr;
    ThumbnailItemWidget *m_pThumbnailItemWidget = nullptr;

    int     m_nCurrentRow = -1;     //  上一次　所在的行，避免多次重复点击
};

#endif // THUMBNAILWIDGET_H
