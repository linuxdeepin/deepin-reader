#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include "header/CustomWidget.h"
#include <DWidget>
#include <QMouseEvent>

#include "view/MagnifyingWidget.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The FileViewWidget class
 * @brief   文档显示区域
 */

class FileViewWidget : public CustomWidget
{
    Q_OBJECT
public:
    FileViewWidget(CustomWidget *parent = nullptr);

protected:
    void initWidget() override;

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    MagnifyingWidget *m_pMagnifyingWidget = nullptr;
    bool m_bCanVisible = false; //  放大镜 是否可以显示

    // IObserver interface
public:
    int update(const int &, const QString &) override;
};

#endif // FILEVIEWWIDGET_H
