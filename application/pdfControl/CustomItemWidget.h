/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CUSTOMITEMWIDGET_H
#define CUSTOMITEMWIDGET_H

#include <DHorizontalLine>

#include "CustomControl/CustomWidget.h"
#include "CustomControl/ImageLabel.h"
#include "CustomControl/PageNumberLabel.h"

//  统一设置 image 的widget 抽象类, 相对应的页码

class CustomItemWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CustomItemWidget)

public:
    CustomItemWidget(const QString &, DWidget *parent = nullptr);

signals:
    void sigDeleteItem(const QString &);
    void sigSelectItem(const QString &);

public:
    void setLabelImage(const QImage &);
    void setLabelPage(const int &, const int &nShowPage = 0);

    int nPageIndex() const;
    virtual void imageAdaptView(const int &, const int &);

protected:
    void resizeEvent(QResizeEvent *event) override;

    QString calcText(const QFont &font, const QString &note, const QSize &size);
protected:
    ImageLabel          *m_pPicture = nullptr;          // 承载缩略图的label
    PageNumberLabel     *m_pPageNumber = nullptr;       // 页码label

    int                 m_nPageIndex  = -1;             // 所对应的页码
};

#endif // CUSTOMITEMWIDGET_H
