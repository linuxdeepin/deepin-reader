#ifndef MAGNIFYINGWIDGET_H
#define MAGNIFYINGWIDGET_H

#include <DWidget>
#include <QPaintEvent>

#include "header/CustomWidget.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MagnifyingWidget class
 * @brief   放大镜窗口
 */

class MagnifyingWidget : public CustomWidget
{
    Q_OBJECT
public:
    MagnifyingWidget(CustomWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

    // IObserver interface
public:
    int update(const int &, const QString &) override;
};

#endif // MAGNIFYINGWIDGET_H
