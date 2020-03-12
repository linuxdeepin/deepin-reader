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
#include "ProxyViewDisplay.h"

#include <QJsonObject>
#include <QJsonDocument>

#include "FileViewWidgetPrivate.h"

#include "application.h"
#include "ModuleHeader.h"
#include "WidgetHeader.h"

#include "docview/docummentproxy.h"

using namespace DR_SPACE;

ProxyViewDisplay::ProxyViewDisplay(QObject *parent) : QObject(parent)
{
    fvmPrivate = qobject_cast<FileViewWidgetPrivate *>(parent);
}

void ProxyViewDisplay::OnSetViewHit(const QString &msgContent)
{
    m_nAdapteState = msgContent.toInt();

    onSetWidgetAdapt();
}

//  设置　窗口　自适应　宽＼高　度
void ProxyViewDisplay::onSetWidgetAdapt()
{
    if (m_nAdapteState != Default_State) {
        if (!fvmPrivate->m_pProxy)
            return;

        double dScale = 0.0;
        if (m_nAdapteState == ADAPTE_WIDGET_State) {
            dScale = fvmPrivate->m_pProxy->adaptWidthAndShow(m_nWidth);
        } else if (m_nAdapteState == ADAPTE_HEIGHT_State) {
            dScale = fvmPrivate->m_pProxy->adaptHeightAndShow(m_nHeight);
        }

        if (dScale != 0.0) {
            notifyMsg(MSG_FILE_FIT_SCALE, QString::number(dScale));
        }
    }
}

//  比例调整了, 取消自适应 宽高状态
void ProxyViewDisplay::OnSetViewScale(const QString &msgConent)
{
    m_nScale = msgConent.toInt();

    setScaleRotateViewModeAndShow();

    m_nAdapteState = Default_State;
}

void ProxyViewDisplay::OnSetViewRotate(const QString &msgConent)
{
    int nTemp = msgConent.toInt();
    if (nTemp == 1) { //  右旋转
        m_rotateType++;
    } else {
        m_rotateType--;
    }

    if (m_rotateType > RotateType_270) {
        m_rotateType = RotateType_0;
    } else if (m_rotateType < RotateType_0) {
        m_rotateType = RotateType_270;
    }

    QJsonObject obj;
    obj.insert("content", QString::number(m_rotateType));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_VIEWCHANGE_ROTATE_VALUE, doc.toJson(QJsonDocument::Compact));

    setScaleRotateViewModeAndShow();

    //  旋转之后, 若是 双页显示
    onSetWidgetAdapt();
}

void ProxyViewDisplay::OnSetViewChange(const QString &msgContent)
{
    m_nDoubleShow = msgContent.toInt();
    setScaleRotateViewModeAndShow();
}

void ProxyViewDisplay::setScaleRotateViewModeAndShow()
{
    if (fvmPrivate->m_pProxy) {
        double dScale = m_nScale / 100.0;
        ViewMode_EM em = m_nDoubleShow ? ViewMode_FacingPage : ViewMode_SinglePage;
        fvmPrivate->m_pProxy->setScaleRotateViewModeAndShow(dScale, static_cast<RotateType_EM>(m_rotateType), em);
    }
}

//  通知消息
void ProxyViewDisplay::notifyMsg(const int &msgType, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
}

void ProxyViewDisplay::setHeight(const int &nHeight)
{
    m_nHeight = nHeight;
}

void ProxyViewDisplay::setWidth(const int &nWidth)
{
    m_nWidth = nWidth;
}

void ProxyViewDisplay::setScale(const int &nScale)
{
    m_nScale = nScale;
}

void ProxyViewDisplay::setRotateType(const int &rotateType)
{
    m_rotateType = rotateType;
}

void ProxyViewDisplay::setDoubleShow(const int &nDoubleShow)
{
    m_nDoubleShow = nDoubleShow;
}

void ProxyViewDisplay::setAdapteState(const int &nAdapteState)
{
    m_nAdapteState = nAdapteState;
}
