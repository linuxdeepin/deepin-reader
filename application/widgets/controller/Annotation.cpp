/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
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

#include "Annotation.h"

#include "application.h"
#include "MsgHeader.h"
#include "ModuleHeader.h"

#include "business/AppInfo.h"
#include "docview/docummentproxy.h"

Annotation::Annotation(QObject *parent)
    : QObject(parent)
{}

void Annotation::setProxy(DocummentProxy *proxy)
{
    m_pProxy = proxy;
}

//  删除注释节点
QString Annotation::DeletePageIconAnnotation(const QString &msgContent)
{
    if (m_pProxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 2) {
            QString sUuid = sList.at(0);
            QString sPage = sList.at(1);

            m_pProxy->removeAnnotation(sUuid, sPage.toInt());
            return sUuid;
        }
    }
    return "";
}

//  更新注释节点内容
QString Annotation::UpdatePageIconAnnotation(const QString &msgContent)
{
    if (m_pProxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sUuid = sList.at(0);
            QString sNote = sList.at(1);
            QString sPage = sList.at(2);
            m_pProxy->setAnnotationText(sPage.toInt(), sUuid, sNote);

            return msgContent;
        }
    }

    return "";
}

//  添加 高亮
void Annotation::AddHighLight(const QString &msgContent)
{
    if (m_pProxy) {
        QString nSx = "", nSy = "", nEx = "", nEy = "";
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 4) {
            nSx = sList.at(0);
            nSy = sList.at(1);
            nEx = sList.at(2);
            nEy = sList.at(3);
        } else if (sList.size() == 5) {
            nSx = sList.at(0);
            nSy = sList.at(1);
            nEx = sList.at(2);
            nEy = sList.at(3);
            int iIndex = sList.at(4).toInt();

            QColor color = dApp->m_pAppInfo->getLightColorList().at(iIndex);
            dApp->m_pAppInfo->setSelectColor(color);
        }

        QPoint pStartPoint(nSx.toInt(), nSy.toInt());
        QPoint pEndPoint(nEx.toInt(), nEy.toInt());
        QColor color = dApp->m_pAppInfo->selectColor();

        m_pProxy->addAnnotation(pStartPoint, pEndPoint, color);
    }
}

QString Annotation::AddHighLightAnnotation(const QString &msgContent)
{
    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 6) {
        QString nSx = sList.at(0);
        QString nSy = sList.at(1);
        QString nEx = sList.at(2);
        QString nEy = sList.at(3);
        QString sNote = sList.at(4);
        QString sPage = sList.at(5);

        if (m_pProxy) {
            QPoint pStartPoint(nSx.toInt(), nSy.toInt());
            QPoint pEndPoint(nEx.toInt(), nEy.toInt());
            QColor color = dApp->m_pAppInfo->selectColor();

            QString strUuid = m_pProxy->addAnnotation(pStartPoint, pEndPoint, color);
            if (strUuid != "") {
                m_pProxy->setAnnotationText(sPage.toInt(), strUuid, sNote);
                return strUuid.trimmed() + Constant::sQStringSep + sNote.trimmed() + Constant::sQStringSep + sPage;
            }
        }
    }
    return "";
}

//  移除高亮颜色, 若是有注释, 则删除注释
QString Annotation::RemoveHighLight(const QString &msgContent)
{
    if (m_pProxy) {
        QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (contentList.size() == 2) {
            QString sX = contentList.at(0);
            QString sY = contentList.at(1);

            QPoint tempPoint(sX.toInt(), sY.toInt());

            QString sUuid = m_pProxy->removeAnnotation(tempPoint);
            if (sUuid != "") {
                return sUuid;
            }
        }
    }
    return "";
}

//  更新高亮颜色
void Annotation::ChangeAnnotationColor(const QString &msgContent)
{
    if (m_pProxy) {
        QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (contentList.size() == 3) {
            QString sIndex = contentList.at(0);
            QString sUuid = contentList.at(1);
            QString sPage = contentList.at(2);

            int iIndex = sIndex.toInt();
            QColor color = dApp->m_pAppInfo->getLightColorList().at(iIndex);

            m_pProxy->changeAnnotationColor(sPage.toInt(), sUuid, color);     //  更新高亮顏色,  是对文档进行了操作
        }
    }
}

//  删除注释
QString Annotation::RemoveAnnotation(const QString &msgContent)
{
    if (m_pProxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 2) {
            QString sUuid = sList.at(0);
            QString sPage = sList.at(1);
            m_pProxy->removeAnnotation(sUuid, sPage.toInt());

            return sUuid;
        }
    }
    return "";
}


void Annotation::UpdateAnnotationText(const QString &msgContent)
{
    if (m_pProxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sUuid = sList.at(0);
            QString sText = sList.at(1);
            QString sPage = sList.at(2);

            m_pProxy->setAnnotationText(sPage.toInt(), sUuid, sText);
        }
    }
}

//  增加页面注释图标
void Annotation::AddPageIconAnnotation(const QString &msgContent)
{
    if (m_pProxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sUuid = sList.at(0);
            QString  sNote = sList.at(1);
            QString sPage = sList.at(2);
            m_pProxy->setAnnotationText(sPage.toInt(), sUuid, sNote);
        }
    }
}
