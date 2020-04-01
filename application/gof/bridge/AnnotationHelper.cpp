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

#include "AnnotationHelper.h"

//#include "application.h"

//#include "business/AppInfo.h"
//#include "docview/docummentproxy.h"

//#include "CentralDocPage.h"

AnnotationHelper::AnnotationHelper(QObject *parent)
    : QObject(parent)
{
//    m_pMsgList = {};
}

//QString AnnotationHelper::qDealWithData(const int &msgType, const QString &msgContent)
//{
//    QString sRes = "";
//    if (msgType == MSG_NOTE_PAGE_ADD_CONTENT) {                 //  新增 页面注释
//        AddPageIconAnnotation(msgContent);
//    } else if (msgType == MSG_NOTE_PAGE_DELETE_CONTENT) {       //  删除页面注释
//        __DeletePageIconAnnotation(msgContent, sRes);
//    } else if (msgType == MSG_NOTE_PAGE_UPDATE_CONTENT) {       //  更新页面注释
//        __UpdatePageIconAnnotation(msgContent);
//    } else if (msgType == MSG_NOTE_DELETE_CONTENT) {            //  刪除高亮注释
//        __RemoveAnnotation(msgContent, sRes);
//    } else if (msgType == MSG_NOTE_UPDATE_CONTENT) {            //  更新高亮注释
//        __UpdateAnnotationText(msgContent);
//    } else if (msgType == MSG_NOTE_REMOVE_HIGHLIGHT) {  //  移除高亮注释 的高亮
//        __RemoveHighLight(msgContent, sRes);
//    } else if (msgType == MSG_NOTE_UPDATE_HIGHLIGHT_COLOR) {  //  更新高亮颜色
//        __ChangeAnnotationColor(msgContent);
//    } else if (msgType == MSG_NOTE_ADD_HIGHLIGHT_COLOR) {                 //  添加高亮
//        __AddHighLight(msgContent);
//    } else if (msgType == MSG_NOTE_ADD_HIGHLIGHT_NOTE) {            //  添加高亮注释
//        __AddHighLightAnnotation(msgContent, sRes);
//    }

//    int nRes = MSG_NO_OK;
//    if (m_pMsgList.contains(msgType)) {
//        nRes = MSG_OK;
//    }

//    QJsonObject obj;
//    obj.insert("return", nRes);
//    obj.insert("value", sRes);

//    QJsonDocument doc(obj);

//    return doc.toJson(QJsonDocument::Compact);
//}

////  删除注释节点
//void AnnotationHelper::__DeletePageIconAnnotation(const QString &msgContent, QString &sUuid)
//{
//    CentralDocPage *pMtwe = CentralDocPage::Instance();

//    DocummentProxy *_proxy = pMtwe->getCurFileAndProxy();
//    if (_proxy) {
//        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
//        if (sList.size() == 2) {
//            sUuid = sList.at(0);
//            QString sPage = sList.at(1);

//            _proxy->removeAnnotation(sUuid, sPage.toInt());
//            pMtwe->SetFileChange();
//        }
//    }
//}


////  更新注释节点内容
//void AnnotationHelper::__UpdatePageIconAnnotation(const QString &msgContent)
//{
//    CentralDocPage *pMtwe = CentralDocPage::Instance();

//    DocummentProxy *_proxy = pMtwe->getCurFileAndProxy();
//    if (_proxy) {
//        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
//        if (sList.size() == 3) {
//            QString sUuid = sList.at(0);
//            QString sNote = sList.at(1);
//            QString sPage = sList.at(2);
//            _proxy->setAnnotationText(sPage.toInt(), sUuid, sNote);
//            pMtwe->SetFileChange();
//        }
//    }
//}

//void AnnotationHelper::__AddHighLight(const QString &msgContent)
//{
//    CentralDocPage *pMtwe = CentralDocPage::Instance();

//    DocummentProxy *_proxy = pMtwe->getCurFileAndProxy();
//    if (_proxy) {
//        QString nSx = "", nSy = "", nEx = "", nEy = "";
//        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
//        if (sList.size() == 4) {
//            nSx = sList.at(0);
//            nSy = sList.at(1);
//            nEx = sList.at(2);
//            nEy = sList.at(3);
//        } else if (sList.size() == 5) {
//            nSx = sList.at(0);
//            nSy = sList.at(1);
//            nEx = sList.at(2);
//            nEy = sList.at(3);
//            int iIndex = sList.at(4).toInt();

//            QColor color = dApp->m_pAppInfo->getLightColorList().at(iIndex);
//            dApp->m_pAppInfo->setSelectColor(color);
//        }

//        QPoint pStartPoint(nSx.toInt(), nSy.toInt());
//        QPoint pEndPoint(nEx.toInt(), nEy.toInt());
//        QColor color = dApp->m_pAppInfo->selectColor();

//        _proxy->addAnnotation(pStartPoint, pEndPoint, color);
//        pMtwe->SetFileChange();
//    }
//}

//void AnnotationHelper::__AddHighLightAnnotation(const QString &msgContent, QString &sRes)
//{
//    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
//    if (sList.size() == 6) {
//        QString nSx = sList.at(0);
//        QString nSy = sList.at(1);
//        QString nEx = sList.at(2);
//        QString nEy = sList.at(3);
//        QString sNote = sList.at(4);
//        QString sPage = sList.at(5);

//        CentralDocPage *pMtwe = CentralDocPage::Instance();

//        DocummentProxy *_proxy = pMtwe->getCurFileAndProxy();
//        if (_proxy) {
//            QPoint pStartPoint(nSx.toInt(), nSy.toInt());
//            QPoint pEndPoint(nEx.toInt(), nEy.toInt());
//            QColor color = dApp->m_pAppInfo->selectColor();

//            QString strUuid = _proxy->addAnnotation(pStartPoint, pEndPoint, color);
//            if (strUuid != "") {
//                _proxy->setAnnotationText(sPage.toInt(), strUuid, sNote);

//                sRes = strUuid.trimmed() + Constant::sQStringSep + sNote.trimmed() + Constant::sQStringSep + sPage;
//            }
//            pMtwe->SetFileChange();
//        }
//    }
//}

//void AnnotationHelper::__RemoveHighLight(const QString &msgContent, QString &sRes)
//{
//    CentralDocPage *pMtwe = CentralDocPage::Instance();

//    DocummentProxy *_proxy = pMtwe->getCurFileAndProxy();
//    if (_proxy) {
//        QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
//        if (contentList.size() == 2) {
//            QString sX = contentList.at(0);
//            QString sY = contentList.at(1);

//            QPoint tempPoint(sX.toInt(), sY.toInt());

//            QString sUuid = _proxy->removeAnnotation(tempPoint);
//            if (sUuid != "") {
//                sRes = sUuid;
//            }
//            pMtwe->SetFileChange();
//        }
//    }
//}

////  更新高亮颜色
//void AnnotationHelper::__ChangeAnnotationColor(const QString &msgContent)
//{
//    CentralDocPage *pMtwe = CentralDocPage::Instance();

//    DocummentProxy *_proxy = pMtwe->getCurFileAndProxy();
//    if (_proxy) {
//        QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
//        if (contentList.size() == 3) {
//            QString sIndex = contentList.at(0);
//            QString sUuid = contentList.at(1);
//            QString sPage = contentList.at(2);

//            int iIndex = sIndex.toInt();
//            QColor color = dApp->m_pAppInfo->getLightColorList().at(iIndex);

//            _proxy->changeAnnotationColor(sPage.toInt(), sUuid, color);     //  更新高亮顏色,  是对文档进行了操作
//            pMtwe->SetFileChange();
//        }
//    }
//}

//void AnnotationHelper::__RemoveAnnotation(const QString &msgContent, QString &sUuid)
//{
//    CentralDocPage *pMtwe = CentralDocPage::Instance();

//    DocummentProxy *_proxy = pMtwe->getCurFileAndProxy();
//    if (_proxy) {
//        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
//        if (sList.size() == 2) {
//            sUuid = sList.at(0);
//            QString sPage = sList.at(1);
//            _proxy->removeAnnotation(sUuid, sPage.toInt());
//            pMtwe->SetFileChange();
//        }
//    }
//}

//void AnnotationHelper::__UpdateAnnotationText(const QString &msgContent)
//{
//    CentralDocPage *pMtwe = CentralDocPage::Instance();

//    DocummentProxy *_proxy = pMtwe->getCurFileAndProxy();
//    if (_proxy) {
//        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
//        if (sList.size() == 3) {
//            QString sUuid = sList.at(0);
//            QString sText = sList.at(1);
//            QString sPage = sList.at(2);

//            _proxy->setAnnotationText(sPage.toInt(), sUuid, sText);
//            pMtwe->SetFileChange();
//        }
//    }
//}

//void AnnotationHelper::AddPageIconAnnotation(const QString &msgContent)
//{
//    CentralDocPage *pMtwe = CentralDocPage::Instance();

//    DocummentProxy *_proxy = pMtwe->getCurFileAndProxy();
//    if (_proxy) {
//        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
//        if (sList.size() == 3) {
//            QString sUuid = sList.at(0);
//            QString  sNote = sList.at(1);
//            QString sPage = sList.at(2);
//            _proxy->setAnnotationText(sPage.toInt(), sUuid, sNote);
//            QString t_str = sUuid.trimmed() + Constant::sQStringSep + sNote.trimmed() + Constant::sQStringSep + sPage;
//            pMtwe->SetFileChange();
//        }
//    }
//}
