#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include "header/CustomWidget.h"
#include <QMouseEvent>

#include "view/MagnifyingWidget.h"


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
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    MagnifyingWidget *m_pMagnifyingWidget = nullptr;
    bool m_bCanVisible = false; //  放大镜 是否可以显示

    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int update(const int &, const QString &) override;
};

#endif // FILEVIEWWIDGET_H
