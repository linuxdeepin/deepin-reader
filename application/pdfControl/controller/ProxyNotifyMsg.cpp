/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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
#include "ProxyNotifyMsg.h"

#include "pdfControl/SheetBrowserPDFPrivate.h"

#include "pdfControl/SheetBrowserPDF.h"
#include <QJsonObject>
#include <QJsonDocument>

#include "application.h"

#include "MsgHeader.h"
#include "ModuleHeader.h"
#include "WidgetHeader.h"

using namespace DR_SPACE;

ProxyNotifyMsg::ProxyNotifyMsg(QObject *parent) : QObject(parent)
{
    _fvwParent = qobject_cast<SheetBrowserPDFPrivate *>(parent);
}

//  文档书签状态改变
void ProxyNotifyMsg::slotBookMarkStateChange(int nPage, bool bState)
{
    if (!bState) {
        emit _fvwParent->q_func()->sigBookMarkMsg(MSG_OPERATION_DELETE_BOOKMARK, QString::number(nPage));
    } else {
        emit _fvwParent->q_func()->sigBookMarkMsg(MSG_OPERATION_ADD_BOOKMARK, QString::number(nPage));
    }
}

