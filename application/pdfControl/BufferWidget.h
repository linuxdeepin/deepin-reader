#ifndef BUFFERWIDGET_H
#define BUFFERWIDGET_H

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <DSpinner>

#include "CustomControl/CustomWidget.h"

class BufferWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BufferWidget)
public:
    explicit BufferWidget(CustomWidget *parent = nullptr);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    QVBoxLayout *m_pVLayout = nullptr;  // 承载spinner的垂直布局
    QHBoxLayout *m_pHLayout = nullptr;  // 承载spinner的水平布局
    DSpinner *m_pSpinner = nullptr;     // 缓冲动画
};

#endif  // BUFFERWIDGET_H
