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
#include "CustomClickLabel.h"

CustomClickLabel::CustomClickLabel(const QString &text, DWidget *parent, Qt::WindowFlags f)
    : DLabel(text, parent)
{
//setWindowFlag();
}

//  设置 主题颜色
void CustomClickLabel::setThemePalette()
{
//    Dtk::Gui::DPalette plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
//    plt.setColor(Dtk::Gui::DPalette::WindowText, plt.color(Dtk::Gui::DPalette::TextTips));
//    setPalette(plt);
    setForegroundRole(DPalette::BrightText);
}

/**
* @brief CustomClickLabel::mousePressEvent
* 自定义label点击事件
* @param event
*/

void CustomClickLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }

    DLabel::mousePressEvent(event);
}
