#ifndef THUMBNAILITEMWIDGET_H
#define THUMBNAILITEMWIDGET_H
#include <DWidget>
#include <DLabel>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE


/**
 * @brief The ThumbnailItemWidget class
 * @brief   缩略图中的item
 */


class ThumbnailItemWidget  : public DWidget
{
    Q_OBJECT
public:
    ThumbnailItemWidget();

public:
    void setContantLabelPixmap(const QString&);
    void setPageLabelText(const QString&);

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

#endif // THUMBNAILITEMWIDGET_H
