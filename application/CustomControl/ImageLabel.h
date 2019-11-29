#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <DWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class ImageLabel : public DLabel
{
    Q_OBJECT
public:
    ImageLabel(DWidget *parent = nullptr);

public:
    void setSelect(const bool &select);
    inline void setRadius(const int radius)
    {
        m_nRadius = radius;
    }
    inline void setBookMarkStatus(bool bshow)
    {
        m_bshowbookmark=bshow;
    }

    inline void setBackgroundPix(QPixmap pixmap)
    {
        m_background = pixmap;
        m_bSetBp = true;
    }

protected:
    void  paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    bool m_bSelect = false;
    int m_nRadius = 8;         // 圆角
    bool m_bshowbookmark=false;       //显示书签

    QPixmap m_background;        // 缩略图
    bool m_bSetBp = false;       // 是否设置缩略图
};

#endif // IMAGELABEL_H
