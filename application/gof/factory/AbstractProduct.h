#ifndef ABSTRACTPRODUCT_H
#define ABSTRACTPRODUCT_H

#include <DWidget>
#include <QString>

DWIDGET_USE_NAMESPACE;

class AbstractProduct
{
public:
    virtual ~AbstractProduct();
    virtual int qDealWithData(const int&, const QString&) = 0;

protected:
    explicit AbstractProduct();
};

#endif // ABSTRACTPRODUCT_H
