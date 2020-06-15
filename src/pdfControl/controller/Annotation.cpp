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

#include "pdfControl/SheetBrowserPDFPrivate.h"

#include "application.h"
#include "MsgHeader.h"
#include "ModuleHeader.h"
#include "ProxyData.h"

#include "business/AppInfo.h"
#include "pdfControl/docview/docummentproxy.h"

#include "pdfControl/SheetBrowserPDF.h"

Annotation::Annotation(QObject *parent)
    : QObject(parent)
{
    fvmPrivate = qobject_cast<SheetBrowserPDFPrivate *>(parent);
}

void Annotation::handleNote(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_NOTE_REMOVE_HIGHLIGHT) {                 //  移除高亮注释 的高亮
        RemoveHighLight(msgContent);
    } else if (msgType == MSG_NOTE_UPDATE_HIGHLIGHT_COLOR) {    //  更新高亮颜色
        ChangeAnnotationColor(msgContent);
    } else if (msgType == MSG_NOTE_ADD_HIGHLIGHT_COLOR) {       //  添加高亮
        AddHighLight(msgContent);
    } else if (msgType == MSG_NOTE_DELETE_CONTENT) {            //  刪除高亮注释
        RemoveAnnotation(msgContent);
    } else if (msgType == MSG_NOTE_UPDATE_CONTENT) {            //  更新高亮注释
        UpdateAnnotationText(msgContent);
    } else if (msgType == MSG_NOTE_PAGE_ADD_CONTENT) {          //  新增 页面注释
        AddPageIconAnnotation(msgContent);
    } else if (msgType == MSG_NOTE_PAGE_UPDATE_CONTENT) {       //  更新页面注释
        UpdatePageIconAnnotation(msgContent);
    } else if (msgType == MSG_NOTE_PAGE_DELETE_CONTENT) {       //  删除页面注释
        DeletePageIconAnnotation(msgContent);
    }
}

//  删除注释节点
void Annotation::DeletePageIconAnnotation(const QString &msgContent)
{
    if (fvmPrivate->m_pProxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sUuid = sList.at(0);
            QString sPage = sList.at(1);
            int firstAdd = sList.at(2).toInt();

            fvmPrivate->m_pProxy->removeAnnotation(sUuid, sPage.toInt());

            if (sUuid != "") {
                if (firstAdd == 0)
                    emit fvmPrivate->q_func()->sigAnntationMsg(MSG_NOTE_PAGE_DELETE_ITEM, sUuid);
                fvmPrivate->m_pProxyData->setFileChanged(true);
            }
        }
    }
}

//  更新注释节点内容
void Annotation::UpdatePageIconAnnotation(const QString &msgContent)
{
    if (fvmPrivate->m_pProxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sUuid = sList.at(0);
            QString sNote = sList.at(1);
            QString sPage = sList.at(2);
            fvmPrivate->m_pProxy->setAnnotationText(sPage.toInt(), sUuid, sNote);

            emit fvmPrivate->q_func()->sigAnntationMsg(MSG_NOTE_PAGE_UPDATE_ITEM, msgContent);

            fvmPrivate->m_pProxyData->setFileChanged(true);
        }
    }
}

//  添加 高亮
void Annotation::AddHighLight(const QString &msgContent)
{
    if (fvmPrivate->m_pProxy) {
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
            fvmPrivate->setSelectColor(color);
        }

        QPoint pStartPoint(nSx.toInt(), nSy.toInt());
        QPoint pEndPoint(nEx.toInt(), nEy.toInt());
        QColor color = fvmPrivate->selectColor();

        fvmPrivate->m_pProxy->addAnnotation(pStartPoint, pEndPoint, color);
        fvmPrivate->m_pProxyData->setFileChanged(true);
        int page = 0;
        page = fvmPrivate->m_pProxy->pointInWhichPage(pStartPoint);
        emit fvmPrivate->q_func()->sigUpdateThumbnail(page);
    }
}

void Annotation::AddHighLightAnnotation(const QString &msgContent)
{
    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 6) {
        QString nSx = sList.at(0);
        QString nSy = sList.at(1);
        QString nEx = sList.at(2);
        QString nEy = sList.at(3);
        QString sNote = sList.at(4);
        QString sPage = sList.at(5);

        if (fvmPrivate->m_pProxy) {
            QPoint pStartPoint(nSx.toInt(), nSy.toInt());
            QPoint pEndPoint(nEx.toInt(), nEy.toInt());
            QColor color = fvmPrivate->selectColor();
            QString strUuid = fvmPrivate->m_pProxy->addAnnotation(pStartPoint, pEndPoint, color);

            if (strUuid != "") {
                fvmPrivate->m_pProxy->setAnnotationText(sPage.toInt(), strUuid, sNote);

                QString sRes = strUuid.trimmed() + Constant::sQStringSep + sNote.trimmed() + Constant::sQStringSep + sPage;
                if (sRes != "") {
                    emit fvmPrivate->q_func()->sigAnntationMsg(MSG_NOTE_ADD_ITEM, sRes);
                    fvmPrivate->m_pProxyData->setFileChanged(true);
                    emit fvmPrivate->q_func()->sigUpdateThumbnail(sPage.toInt());
                }
            }
        }
    }
}

//  移除高亮颜色, 若是有注释, 则删除注释
void Annotation::RemoveHighLight(const QString &msgContent)
{
    if (fvmPrivate->m_pProxy) {
        QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (contentList.size() == 3) {
            QString sX = contentList.at(0);
            QString sY = contentList.at(1);

            QPoint tempPoint(sX.toInt(), sY.toInt());
            QString annoteText{""};
            QString t_strUUid = contentList.at(2);

            bool isHighLight = fvmPrivate->m_pProxy->annotationClicked(tempPoint, annoteText, t_strUUid);

            QString sUuid{""};// = fvmPrivate->m_pProxy->removeAnnotation(tempPoint);

            int curPage = -1;

            curPage  = fvmPrivate->m_pProxy->pointInWhichPage(tempPoint);
            sUuid = contentList.at(2);

            if (sUuid != "") {
                fvmPrivate->m_pProxy->removeAnnotation(sUuid, curPage);
                if (isHighLight) {
                    if (annoteText != "") {
                        emit fvmPrivate->q_func()->sigAnntationMsg(MSG_NOTE_DELETE_ITEM, sUuid);
                    }
                } else {
                    emit fvmPrivate->q_func()->sigAnntationMsg(MSG_NOTE_DELETE_ITEM, sUuid);
                }
                fvmPrivate->m_pProxyData->setFileChanged(true);
                int page = 0;
                page = fvmPrivate->m_pProxy->pointInWhichPage(tempPoint);
                emit fvmPrivate->q_func()->sigUpdateThumbnail(page);
            }
        }
    }
}

//  更新高亮颜色
void Annotation::ChangeAnnotationColor(const QString &msgContent)
{
    if (fvmPrivate->m_pProxy) {
        QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (contentList.size() == 3) {
            QString sIndex = contentList.at(0);
            QString sUuid = contentList.at(1);
            QString sPage = contentList.at(2);

            int iIndex = sIndex.toInt();

            QColor color = dApp->m_pAppInfo->getLightColorList().at(iIndex);
            fvmPrivate->m_pProxy->changeAnnotationColor(sPage.toInt(), sUuid, color);     //  更新高亮顏色,  是对文档进行了操作
//            dApp->m_pAppCfg->setSelectColor(color);
            fvmPrivate->setSelectColor(color);
            fvmPrivate->m_pProxyData->setFileChanged(true);
            emit fvmPrivate->q_func()->sigUpdateThumbnail(sPage.toInt());
        }
    }
}

//  删除注释
void Annotation::RemoveAnnotation(const QString &msgContent)
{
    if (fvmPrivate->m_pProxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 2) {
            QString sUuid = sList.at(0);
            QString sPage = sList.at(1);
            fvmPrivate->m_pProxy->removeAnnotation(sUuid, sPage.toInt());
            emit fvmPrivate->q_func()->sigAnntationMsg(MSG_NOTE_DELETE_ITEM, sUuid);
            fvmPrivate->m_pProxyData->setFileChanged(true);
            emit fvmPrivate->q_func()->sigUpdateThumbnail(sPage.toInt());
        }
    }
}


void Annotation::UpdateAnnotationText(const QString &msgContent)
{
    if (fvmPrivate->m_pProxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sUuid = sList.at(0);
            QString sText = sList.at(1);
            QString sPage = sList.at(2);

            fvmPrivate->m_pProxy->setAnnotationText(sPage.toInt(), sUuid, sText);

            emit fvmPrivate->q_func()->sigAnntationMsg(MSG_NOTE_UPDATE_ITEM, msgContent);
            fvmPrivate->m_pProxyData->setFileChanged(true);
        }
    }
}

//  增加页面注释图标
void Annotation::AddPageIconAnnotation(const QString &msgContent)
{
    if (fvmPrivate->m_pProxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sUuid = sList.at(0);
            QString  sNote = sList.at(1);
            QString sPage = sList.at(2);
            fvmPrivate->m_pProxy->setAnnotationText(sPage.toInt(), sUuid, sNote);

            emit fvmPrivate->q_func()->sigAnntationMsg(MSG_NOTE_PAGE_ADD_ITEM, msgContent);
            fvmPrivate->m_pProxyData->setFileChanged(true);
        }
    }
}


