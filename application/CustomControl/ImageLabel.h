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

protected:
    void  paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    bool m_bSelect = false;
};

#endif // IMAGELABEL_H
