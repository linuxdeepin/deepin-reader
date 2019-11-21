#ifndef PAGENUMBERLABEL_H
#define PAGENUMBERLABEL_H

#include <DWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class PageNumberLabel : public DLabel
{
    Q_OBJECT
public:
    PageNumberLabel(DWidget *parent = nullptr);

public:
    void setSelect(const bool &select);

protected:
//    void  paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
//    bool m_bSelect = false;
};

#endif // PAGENUMBERLABEL_H
