#ifndef CUSTOMITEMWIDGET_H
#define CUSTOMITEMWIDGET_H

#include "subjectObserver/CustomWidget.h"
#include "CustomLabel.h"

//  统一设置 image 的widget 抽象类

class CustomItemWidget : public CustomWidget
{
    Q_OBJECT
public:
    CustomItemWidget(const QString &, CustomWidget *parent = nullptr);

public:
    void setLabelImage(const QImage &);

protected:
    CustomLabel *m_pPicture = nullptr;
};

#endif // CUSTOMITEMWIDGET_H
