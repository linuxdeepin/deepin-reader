#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <DWidget>
#include <DListWidget>
#include <QListWidgetItem>
#include <DLabel>
#include <QVBoxLayout>
#include <DPushButton>
#include <QDebug>

DWIDGET_USE_NAMESPACE

/*
*缩略图列表页面
*/
class ItemWidget;
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
    QVBoxLayout * m_pThumbnailVBoxLayout = nullptr;
    DLabel * m_pThumbnailPageLabel = nullptr;

    DWidget * m_itemWidget = nullptr;
    DLabel * m_pSonWidgetPageLabel = nullptr;
};

/*
*ItemWidget 每个Ｉｔｅｍ中的子页面
*/
class ItemWidget : public DWidget
{
public:
    ItemWidget(DWidget *parent = nullptr);
    ~ItemWidget();

public:
    void setContantLabelPixmap(QString);
    void setPageLabelText(QString);
    inline DWidget *  getSonWidget()
    {
        if(m_sonWidget != nullptr){
            return m_sonWidget;
        }else {
            return nullptr;
        }
    }

    inline DLabel *  getPageLabel()
    {
        if(m_pPageLabel != nullptr){
            return m_pPageLabel;
        }else {
            return nullptr;
        }
    }
private:
    void initWidget();

private:
    DLabel * m_pContantLabel = nullptr;
    DLabel * m_pPageLabel = nullptr;
    DWidget * m_sonWidget = nullptr;
    QVBoxLayout * m_pVLayout = nullptr;
};

#endif // THUMBNAILWIDGET_H
