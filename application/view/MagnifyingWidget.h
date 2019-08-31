#ifndef MAGNIFYINGWIDGET_H
#define MAGNIFYINGWIDGET_H

#include <DWidget>
#include <QPaintEvent>

#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MagnifyingWidget class
 * @brief   放大镜窗口
 */

class MagnifyingWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT
public:
    MagnifyingWidget(DWidget *parent = nullptr);
    ~MagnifyingWidget() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    MsgSubject *m_pMsgSubject = nullptr;

    // IObserver interface
public:
    int update(const int &, const QString &) override;
};

#endif // MAGNIFYINGWIDGET_H
