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
};

#endif // PAGENUMBERLABEL_H
