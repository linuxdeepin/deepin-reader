/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     duanxiaohui<duanxiaohui@uniontech.com>
 *
 * Maintainer: duanxiaohui<duanxiaohui@uniontech.com>
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
#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <DWidget>

DWIDGET_USE_NAMESPACE
class BaseWidget : public DWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BaseWidget)

public:
    explicit BaseWidget(DWidget *parent = nullptr);
    virtual ~BaseWidget() {}

public:
    /**
     * @brief CustomWidget::adaptWindowSize
     * 缩略图列表自适应视窗大小
     * @param scale  缩放因子 大于0的数
     */
    virtual void adaptWindowSize(const double &) {}

    /**
     * @brief CustomWidget::updateThumbnail
     * 刷新相应的缩略图
     * @param index 页码数，从0开始
     */
    virtual void updateThumbnail(const int &, bool clear = false) {}

protected:
    /**
     * @brief updateWidgetTheme
     * 主题变换更新
     */
    void updateWidgetTheme();

protected:
    bool bIshandOpenSuccess = false;
};

#endif  // CUSTOMWIDGET_H
