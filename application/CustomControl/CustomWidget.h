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
#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <DFontSizeManager>
#include <DGuiApplicationHelper>
#include <DPalette>
#include <DWidget>

#include "application.h"

#include "CustomMenu.h"
#include "utils/utils.h"
#include "ModuleHeader.h"
#include "MsgHeader.h"

#include <QDebug>


DWIDGET_USE_NAMESPACE

/**
 * @brief The CustomWidget class
 * @brief   封装 DWidget 和 观察者， 便于 继承 DWidget
 *          实现 相应的消息处理， 更关注于 对应的业务处理
 */

const int FIRST_LOAD_PAGES = 20;
const int LEFTMINWIDTH = 266;//226  >>  266
const int LEFTMAXWIDTH = 380;
const int LEFTNORMALWIDTH = 266;//226  >>  266

class CustomWidgetPrivate;

class CustomWidget : public DWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CustomWidget)
    Q_DECLARE_PRIVATE(CustomWidget)

public:
    CustomWidget(DWidget *parent = nullptr);
    virtual ~CustomWidget();

public:
    virtual void adaptWindowSize(const double &);

    virtual void updateThumbnail(const int &);

protected:
    CustomWidget(CustomWidgetPrivate &, DWidget *parent = nullptr);

    virtual void initWidget() = 0;

    void updateWidgetTheme();

protected:
    CustomWidgetPrivate    *d_ptr = nullptr;
};

#endif  // CUSTOMWIDGET_H
