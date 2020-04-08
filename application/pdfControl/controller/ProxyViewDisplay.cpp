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

#include "pdfControl/SheetBrowserPDFPrivate.h"

#include "application.h"
#include "ModuleHeader.h"
#include "WidgetHeader.h"

#include "docview/docummentproxy.h"

using namespace DR_SPACE;

ProxyViewDisplay::ProxyViewDisplay(QObject *parent) : QObject(parent)
{
    fvmPrivate = qobject_cast<SheetBrowserPDFPrivate *>(parent);
}

int ProxyViewDisplay::setViewRotateLeft()
{
    m_rotateType--;

    if (m_rotateType < RotateType_0) {
        m_rotateType = RotateType_270;
    }

    setScaleRotateViewModeAndShow();

    onSetWidgetAdapt();

    return m_rotateType;
}

int ProxyViewDisplay::setViewRotateRight()
{
    m_rotateType++;

    if (m_rotateType > RotateType_270) {
        m_rotateType = RotateType_0;
    }

    setScaleRotateViewModeAndShow();

    onSetWidgetAdapt();

    return m_rotateType;
}

void ProxyViewDisplay::OnSetViewHit(const QString &msgContent)
{
    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 2) {
        m_nAdapteState = sList.at(0).toInt();
    }

    onSetWidgetAdapt();
}

//  设置　窗口　自适应　宽＼高　度
int ProxyViewDisplay::onSetWidgetAdapt()
{
    if (m_nAdapteState != Default_State) {

        if (!fvmPrivate->m_pProxy)
            return -1;

        double dScale = 0.0;
        if (m_nAdapteState == ADAPTE_WIDGET_State) {
            dScale = fvmPrivate->m_pProxy->adaptWidthAndShow(m_nWidth);
            return dScale * 100;
        } else if (m_nAdapteState == ADAPTE_HEIGHT_State) {
            dScale = fvmPrivate->m_pProxy->adaptHeightAndShow(m_nHeight);
            return dScale * 100;
        }
    }

    return -1;
}

//  比例调整了, 取消自适应 宽高状态
void ProxyViewDisplay::OnSetViewScale(const QString &msgConent)
{
    QStringList sList = msgConent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 2) {
        if (sList.at(1).toInt() == 0) {
            return;
        }
        m_nScale = sList.at(0).toDouble();
        setScaleRotateViewModeAndShow();

        m_nAdapteState = Default_State;
    }
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

bool ProxyViewDisplay::isDoubleShow()
{
    return m_nDoubleShow;
}

void ProxyViewDisplay::setWidth(const int &nWidth)
{
    m_nWidth = nWidth;
}

void ProxyViewDisplay::setScale(const int &nScale)
{
    m_nScale = nScale;
}

int ProxyViewDisplay::setFit(int fit)
{
    m_nAdapteState = fit;
    return onSetWidgetAdapt();
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
