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

protected:
    void  paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    bool m_bSelect = false;
    int m_nRadius = 8;         // 圆角
};

#endif // IMAGELABEL_H
