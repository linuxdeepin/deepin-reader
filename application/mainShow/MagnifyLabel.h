#ifndef MAGNIFYLABEL_H
#define MAGNIFYLABEL_H

#include <DLabel>

DWIDGET_USE_NAMESPACE

class MagnifyLabel : public DLabel
{
public:
    MagnifyLabel(DWidget *parent = nullptr);

private:
    void initBcakLabel();
};

#endif // MAGNIFYLABEL_H
