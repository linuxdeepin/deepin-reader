#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"
#include <DWidget>
#include <QMouseEvent>

#include "view/MagnifyingWidget.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The FileViewWidget class
 * @brief   文档显示区域
 */

class FileViewWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT
public:
    FileViewWidget(DWidget *parent = nullptr);
    ~FileViewWidget() override;

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    MsgSubject *m_pMsgSubject = nullptr;

    MagnifyingWidget *m_pMagnifyingWidget = nullptr;
    bool m_bCanVisible = false; //  放大镜 是否可以显示

    // IObserver interface
public:
    int update(const int &, const QString &) override;
};

#endif // FILEVIEWWIDGET_H
