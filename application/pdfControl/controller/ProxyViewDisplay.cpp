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
#include "pdfControl/SheetBrowserPDFPrivate.h"
#include "application.h"
#include "ModuleHeader.h"
#include "docview/docummentproxy.h"

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

//  设置　窗口　自适应　宽＼高　度
double ProxyViewDisplay::onSetWidgetAdapt()
{
    if (m_nAdapteState != NO_ADAPTE_State) {
        if (!fvmPrivate->m_pProxy)
            return m_nScale;

        double dScale = 0.0;
        if (m_nAdapteState == ADAPTE_WIDGET_State) {
            dScale = fvmPrivate->m_pProxy->adaptWidthAndShow(m_nWidth);
            return dScale * 100;
        } else if (m_nAdapteState == ADAPTE_HEIGHT_State) {
            dScale = fvmPrivate->m_pProxy->adaptHeightAndShow(m_nHeight);
            return dScale * 100;
        }
    }

    return m_nScale;
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

void ProxyViewDisplay::setScale(const double &nScale)
{
    m_nScale = nScale;
}

double ProxyViewDisplay::setFit(int fit)
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
