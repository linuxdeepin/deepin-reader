#ifndef CUSTOMITEMWIDGET_H
#define CUSTOMITEMWIDGET_H

#include "subjectObserver/CustomWidget.h"
#include "CustomLabel.h"

//  统一设置 image 的widget 抽象类, 相对应的页码

class CustomItemWidget : public CustomWidget
{
    Q_OBJECT
public:
    CustomItemWidget(const QString &, CustomWidget *parent = nullptr);

public:
    void setLabelImage(const QImage &);
    void setLabelPage(const int &, const int &nShowPage = 0);

    int nPageIndex() const;

protected:
    CustomLabel *m_pPicture = nullptr;   // 承载缩略图的label
    DLabel      *m_pPageNumber = nullptr;      // 页码label
    int         m_nPageIndex  = -1;      // 所对应的页码
};

#endif // CUSTOMITEMWIDGET_H
