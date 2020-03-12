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
#include <DWidgetUtil>

#include "CustomWidgetPrivate.h"

CustomWidget::CustomWidget(const QString &name, DWidget *parent)
    : DWidget(parent), d_ptr(new CustomWidgetPrivate(this))
{
    m_strObserverName = name;
    setWindowFlags(Qt::FramelessWindowHint);
    setFocusPolicy(Qt::StrongFocus);
    setObjectName(name);
    setAutoFillBackground(true);
    setContextMenuPolicy(Qt::CustomContextMenu);//让widget支持右键菜单事件
}

CustomWidget::CustomWidget(CustomWidgetPrivate &cwp, const QString &name, DWidget *parent)
    : DWidget(parent), d_ptr(&cwp)
{

}

void CustomWidget::updateWidgetTheme()
{
    auto plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(Dtk::Gui::DPalette::Background, plt.color(Dtk::Gui::DPalette::Base));
    setPalette(plt);
}

int CustomWidget::qDealWithShortKey(const QString &)
{
    return MSG_NO_OK;
}

//  通知消息
void CustomWidget::notifyMsg(const int &msgType, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
}

void CustomWidget::showScreenCenter()
{
    Dtk::Widget::moveToCenter(this);
    this->show();
}
