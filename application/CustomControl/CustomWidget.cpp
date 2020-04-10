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
#include "CustomWidget.h"
#include "CustomWidgetPrivate.h"
#include "ModuleHeader.h"

#include <DWidgetUtil>

CustomWidget::CustomWidget(DWidget *parent)
    : DWidget(parent), d_ptr(new CustomWidgetPrivate(this))
{
    setWindowFlags(Qt::FramelessWindowHint);
    setFocusPolicy(Qt::StrongFocus);
    setAutoFillBackground(true);
    setContextMenuPolicy(Qt::CustomContextMenu);//让widget支持右键菜单事件
}

CustomWidget::CustomWidget(CustomWidgetPrivate &cwp, DWidget *parent)
    : DWidget(parent), d_ptr(&cwp)
{

}

void CustomWidget::updateWidgetTheme()
{
    auto plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(Dtk::Gui::DPalette::Background, plt.color(Dtk::Gui::DPalette::Base));
    setPalette(plt);
}

/**
 * @brief CustomWidget::adaptWindowSize
 * 缩略图列表自适应视窗大小
 * @param scale  缩放因子 大于0的数
 */
void CustomWidget::adaptWindowSize(const double &scale)
{

}

/**
 * @brief CustomWidget::updateThumbnail
 * 高亮操作之后要跟换相应的缩略图
 * @param page 页码数，从0开始
 */
void CustomWidget::updateThumbnail(const int &page)
{

}
