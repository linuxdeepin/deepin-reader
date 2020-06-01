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

#include "AppInfo.h"
#include "utils/utils.h"
#include "ModuleHeader.h"
#include <QDebug>

#include <QDir>

AppInfo::AppInfo(QObject *parent)
    : QObject(parent)
{
    m_pSettings = new QSettings(QDir(Utils::getConfigPath()).filePath(ConstantMsg::g_cfg_name), QSettings::IniFormat, parent);

    InitColor();
    InitKeyList();
}

void AppInfo::InitColor()
{
    m_listColor.append(QColor("#FFA503"));
    m_listColor.append(QColor("#FF1C49"));
    m_listColor.append(QColor("#9023FC"));
    m_listColor.append(QColor("#3468FF"));
    m_listColor.append(QColor("#00C7E1"));
    m_listColor.append(QColor("#05EA6B"));
    m_listColor.append(QColor("#FEF144"));
    m_listColor.append(QColor("#D5D5D1"));

    m_selectColor = m_listColor.at(0);
}

void AppInfo::InitKeyList()
{
//    m_pKeyList.append(QKeySequence::HelpContents);            //  dtk 已实现
//    m_pKeyList.append(QKeySequence::Delete);                  //  2020.2.12  delete 不能作快捷键使用
    m_pKeyList.append(QKeySequence::Find);
    m_pKeyList.append(QKeySequence::Open);
    m_pKeyList.append(QKeySequence::Print);
    m_pKeyList.append(QKeySequence::Save);
    m_pKeyList.append(QKeySequence::Copy);

//    m_pKeyList.append(QKeySequence(Qt::Key_Space));
//    m_pKeyList.append(QKeySequence(Qt::Key_F11));             //  全屏暂时取消  2020.1.6    wzx
    m_pKeyList.append(QKeySequence(Qt::Key_Escape));

    m_pKeyList.append(QKeySequence(Qt::ALT | Qt::Key_1));
    m_pKeyList.append(QKeySequence(Qt::ALT | Qt::Key_2));
    m_pKeyList.append(QKeySequence(Qt::ALT | Qt::Key_Z));

    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_1));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_2));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_3));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_B));
//    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_E));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_H));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_I));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_L));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_M));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_R));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_Minus));      //  ctrl+-
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_Equal));      //  ctrl+=

    m_pKeyList.append(QKeySequence(Qt::Key_Left));
    m_pKeyList.append(QKeySequence(Qt::Key_Right));
    m_pKeyList.append(QKeySequence(Qt::Key_Space));

    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_Plus));       //  ctrl++

    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Slash));
}

QColor AppInfo::selectColor() const
{
    return m_selectColor;
}

void AppInfo::setSelectColor(const QColor &color)
{
    m_selectColor = color;
}

QList<QColor> AppInfo::getLightColorList()
{
    return m_listColor;
}

QList<QKeySequence> AppInfo::getKeyList() const
{
    return m_pKeyList;
}

void AppInfo::mousePressLocal(bool &highLight, QPoint &point)
{
    highLight = m_bIsHighLight;
    point = m_point;
}

void AppInfo::setMousePressLocal(const bool &highLight, const QPoint &point)
{
    m_bIsHighLight = highLight;

    QPoint t_point;
    int t_w = point.x();
    int t_h = point.y();

    int screenW =  m_screenRect.width();
    int screenH =  m_screenRect.height();

    int noteWidgetW = m_smallNoteSize.width();
    int noteWidgetH = m_smallNoteSize.height();

    if (t_h + noteWidgetH > screenH) {
        t_h = screenH - noteWidgetH;
    }

    if (t_w + noteWidgetW > screenW) {
        t_w -= noteWidgetW;
    }

    t_point.setX(t_w);
    t_point.setY(t_h);

    m_point = t_point;
}

void AppInfo::setScreenRect(const QRect &rect)
{
    m_screenRect = rect;

//    m_dWidthScale = static_cast<double>((static_cast<double>(rect.width())) / static_cast<double>(m_nWidth));
//    m_dHeightScale = static_cast<double>((static_cast<double>(rect.height())) / static_cast<double>(m_nHeight));
    qInfo() << "        dApp->devicePixelRatio():" << dApp->devicePixelRatio();

}

void AppInfo::setSmallNoteWidgetSize(const QSize &size)
{
    m_smallNoteSize = size;
}

void AppInfo::setAppKeyValue(const int &iKey, const QString &sValue)
{
    // initalize the configuration file.
    QString sKey = QString("%1").arg(iKey);

    QString ssValue = QString("%1").arg(sValue);

    if (sValue == "") {
        m_pSettings->remove(sKey);
    } else {
        m_pSettings->setValue(sKey, ssValue);
    }
}

QString AppInfo::getAppKeyValue(const int &iKey) const
{
    // initalize the configuration file.
    QString sKey = QString("%1").arg(iKey);

    return m_pSettings->value(sKey).toString();
}

/**
 * @brief AppInfo::adaptScreenView     弃用
 * 用于适配各种分辨率  add by dxh   2020-3-24
 * @param width
 * @param height
 */
//void AppInfo::adaptScreenView(int &width, int &height)
//{
//    if (m_dWidthScale > 1.0000)
//    width = static_cast<int>(m_dWidthScale * static_cast<double>(width));
//    if (m_dHeightScale > 1.0000)
//    height = static_cast<int>(m_dHeightScale * static_cast<double>(height));

//    width = static_cast<int>(width * dApp->devicePixelRatio());
//    height = static_cast<int>(height * dApp->devicePixelRatio());
//}
