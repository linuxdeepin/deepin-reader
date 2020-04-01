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
#include "ProxyMouseMove.h"

#include <QDesktopServices>

#include "pdfControl/FileViewWidgetPrivate.h"
#include "ProxyData.h"

#include "business/AppInfo.h"
#include "docview/docummentproxy.h"

#include "gof/bridge/IHelper.h"
#include "widgets/main/MainTabWidgetEx.h"

ProxyMouseMove::ProxyMouseMove(QObject *parent) : QObject(parent)
{
    _fvwParent = qobject_cast<FileViewWidgetPrivate *>(parent);
}

void ProxyMouseMove::mouseMoveEvent(QMouseEvent *event)
{
    QPoint globalPos = event->globalPos();
    //  处于幻灯片模式下
    int nState = MainTabWidgetEx::Instance()->getCurrentState();
    if (nState == SLIDER_SHOW) {
        MainTabWidgetEx::Instance()->showPlayControlWidget();   //  显示 幻灯片 控制
    } else if (nState == Magnifer_State) {    //  当前是放大镜状态
        __ShowMagnifier(globalPos);
    } else if (nState == Handel_State) {  //   手型状态下， 按住鼠标左键 位置进行移动
        if (m_bSelectOrMove) {
            __FilePageMove(globalPos);
        }
    } else if (nState == Default_State) {
        if (m_bSelectOrMove) {  //  鼠标已经按下，　则选中所经过的文字
            __MouseSelectText(globalPos);
        } else {
            __OtherMouseMove(globalPos);
        }
    }
}

//  显示放大镜 数据
void ProxyMouseMove::__ShowMagnifier(const QPoint &clickPoint)
{
    QPoint docGlobalPos = _fvwParent->m_pProxy->global2RelativePoint(clickPoint);

    _fvwParent->m_pProxy->showMagnifier(docGlobalPos);
    _fvwParent->__SetCursor(Qt::BlankCursor);
}

//  鼠标按住, 拖动页面
void ProxyMouseMove::__FilePageMove(const QPoint &endPos)
{
    QPoint mvPoint = m_pMoveStartPoint - endPos;
    int mvX = mvPoint.x();
    int mvY = mvPoint.y();

    _fvwParent->m_pProxy->pageMove(mvX, mvY);

    _fvwParent->__SetCursor(QCursor(Qt::OpenHandCursor));

    m_pMoveStartPoint = endPos;
}

//  选中文本
void ProxyMouseMove::__MouseSelectText(const QPoint &clickPoint)
{
    QPoint docGlobalPos = _fvwParent->m_pProxy->global2RelativePoint(clickPoint);

    _fvwParent->m_pProxyData->setEndSelectPoint(docGlobalPos);
    _fvwParent->m_pProxy->mouseSelectText(_fvwParent->m_pProxyData->getStartPoint(), docGlobalPos);
}

//  其余 鼠标移动
void ProxyMouseMove::__OtherMouseMove(const QPoint &globalPos)
{
    QPoint docGlobalPos = _fvwParent->m_pProxy->global2RelativePoint(globalPos);

    //  首先判断文档划过属性
    auto pLink = _fvwParent->m_pProxy->mouseBeOverLink(docGlobalPos);
    if (pLink) {
        _fvwParent->__SetCursor(QCursor(Qt::PointingHandCursor));
    } else {
        //  先判断是否是页面注释图标
        if (_fvwParent->m_pProxy->mouseovericonAnnotation(docGlobalPos)) {
            __ShowPageNoteWidget(docGlobalPos);
        } else if (_fvwParent->m_pProxy->mouseBeOverText(docGlobalPos)) {
            _fvwParent->m_pProxyData->setIsHandleSelect(true);

            _fvwParent->__SetCursor(QCursor(Qt::IBeamCursor));

            __ShowFileNoteWidget(docGlobalPos);
        } else {
            _fvwParent->m_pProxyData->setIsHandleSelect(false);

            _fvwParent->__SetCursor(QCursor(Qt::ArrowCursor));

            _fvwParent->__CloseFileNoteWidget();
        }
    }
}

//  显示页面注释 提示界面
void ProxyMouseMove::__ShowPageNoteWidget(const QPoint &docPos)
{
    QString sText = "", sUuid = "";
    bool isOk = _fvwParent->m_pProxy->iconAnnotationClicked(docPos, sText, sUuid);
    if (isOk) {
        if (sText != "") {
            _fvwParent->__ShowNoteTipWidget(sText);
        }
    }
}

//  显示 高亮注释 提示界面
void ProxyMouseMove::__ShowFileNoteWidget(const QPoint &docPos)
{

    QString selectText, t_strUUid;
    bool bIsHighLightReleasePoint = _fvwParent->m_pProxy->annotationClicked(docPos, selectText, t_strUUid);
    if (bIsHighLightReleasePoint) {
        int nPage = _fvwParent->m_pProxy->pointInWhichPage(docPos);
        QString sText = "";
        _fvwParent->m_pProxy->getAnnotationText(t_strUUid, sText, nPage);
        if (sText != "") {
            _fvwParent->__ShowNoteTipWidget(sText);
        }
    }
}

void ProxyMouseMove::mousePressEvent(QMouseEvent *event)
{
    int nState = MainTabWidgetEx::Instance()->getCurrentState();

    Qt::MouseButton nBtn = event->button();
    if (nBtn == Qt::RightButton) {  //  右键处理
        //  处于幻灯片模式下
        if (nState == SLIDER_SHOW) {
            MainTabWidgetEx::Instance()->OnExitSliderShow();
            return;
        }

        //  放大镜状态，
        if (nState == Magnifer_State) {
            MainTabWidgetEx::Instance()->OnExitMagnifer();
            return;
        }
    } else if (nBtn == Qt::LeftButton) { // 左键处理
        //  幻灯片模式下, 左键单击 不作任何处理
        if (nState == SLIDER_SHOW)
            return;

        QPoint globalPos = event->globalPos();
        //  当前状态是 手, 先 拖动, 之后 在是否是链接之类
        if (nState == NOTE_ADD_State) {
            __AddIconAnnotation(globalPos);
        } else if (nState == Handel_State) {
            __HandlClicked(globalPos);
        } else {
            __OtherMousePress(globalPos);
        }
    }
}

//  页面添加注释图标
void ProxyMouseMove::__AddIconAnnotation(const QPoint &globalPos)
{
    QPoint docGlobalPos = _fvwParent->m_pProxy->global2RelativePoint(globalPos);

    //  添加之前先判断, 点击处是否 注释图标
    bool bIsIcon = _fvwParent->m_pProxy->mouseovericonAnnotation(docGlobalPos);
    if (bIsIcon) {
        QString sContent = "", sUuid = "";
        if (_fvwParent->m_pProxy->iconAnnotationClicked(docGlobalPos, sContent, sUuid)) {
        }
    } else {
        QString sUuid = _fvwParent->m_pProxy->addIconAnnotation(docGlobalPos);        //  添加注释图标成功
        if (sUuid != "") {
            dApp->m_pAppInfo->setMousePressLocal(false, globalPos);
            int nClickPage = _fvwParent->m_pProxy->pointInWhichPage(docGlobalPos);
            QString strContent = sUuid.trimmed() + Constant::sQStringSep +
                                 QString::number(nClickPage) + Constant::sQStringSep +
                                 QString::number(globalPos.x()) + Constant::sQStringSep +
                                 QString::number(globalPos.y());
            MainTabWidgetEx::Instance()->setCurrentState(Default_State);

            QJsonObject obj;
            obj.insert("content", strContent);
            obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

            QJsonDocument doc(obj);

            notifyMsg(MSG_NOTE_PAGE_SHOW_NOTEWIDGET, doc.toJson(QJsonDocument::Compact));
        } else {
            qWarning() << __FUNCTION__ << "          " << sUuid;;
        }
    }
}

void ProxyMouseMove::__HandlClicked(const QPoint &globalPos)
{
    m_pMoveStartPoint = globalPos;  //  变成手，　需要的是　相对坐标
    m_bSelectOrMove = true;
}

void ProxyMouseMove::__OtherMousePress(const QPoint &globalPos)
{
    QPoint docGlobalPos = _fvwParent->m_pProxy->global2RelativePoint(globalPos);

    //  点击的时候　先判断　点击处　　是否有链接之类
    auto pLink = _fvwParent->m_pProxy->mouseBeOverLink(docGlobalPos);
    if (pLink) {
        __ClickPageLink(pLink);
    } else {
        int nState = MainTabWidgetEx::Instance()->getCurrentState();
        if (nState == Default_State) {
            _fvwParent->m_pProxy->mouseSelectTextClear();  //  清除之前选中的文字高亮

            _fvwParent->__CloseFileNoteWidget();

            if (_fvwParent->m_pProxyData->IsHandleSelect()) {
                m_bSelectOrMove = true;
                _fvwParent->m_pProxyData->setStartPoint(docGlobalPos);
                _fvwParent->m_pProxyData->setEndSelectPoint(docGlobalPos);
            }
        }
    }
}

//  点击 页面能够跳转
void ProxyMouseMove::__ClickPageLink(Page::Link *pLink)
{
    Page::LinkType_EM linkType = pLink->type;
    if (linkType == Page::LinkType_NULL) {

    } else if (linkType == Page::LinkType_Goto) {
        int page = pLink->page - 1;
        dApp->m_pHelper->qDealWithData(MSG_DOC_JUMP_PAGE, QString::number(page));
    } else if (linkType == Page::LinkType_GotoOtherFile) {

    } else if (linkType == Page::LinkType_Browse) {
        QString surlOrFileName = pLink->urlOrFileName;
        QDesktopServices::openUrl(QUrl(surlOrFileName, QUrl::TolerantMode));
    } else if (linkType == Page::LinkType_Execute) {

    }
}

void ProxyMouseMove::mouseReleaseEvent(QMouseEvent *event)
{
    //  幻灯片模式下, 左键单击 不作任何处理
    int nState = MainTabWidgetEx::Instance()->getCurrentState();
    if (nState == SLIDER_SHOW)
        return;

    Qt::MouseButton nBtn = event->button();

    QPoint globalPos = event->globalPos();
    QPoint docGlobalPos = _fvwParent->m_pProxy->global2RelativePoint(globalPos);
    QString selectText, t_strUUid;
    bool bicon = _fvwParent->m_pProxy->iconAnnotationClicked(docGlobalPos, selectText, t_strUUid);
    //  放大镜状态， 右键则取消放大镜 并且 直接返回
    if (m_bSelectOrMove && !bicon) {
        //判断鼠标左键松开的位置有没有高亮

        dApp->m_pAppInfo->setMousePressLocal(false, globalPos);
        //添加其实结束point是否为同一个，不是同一个说明不是点击可能是选择文字
        if (nBtn == Qt::LeftButton && docGlobalPos == _fvwParent->m_pProxyData->getStartPoint()) {
            // 判断鼠标点击的地方是否有高亮
            bool bIsHighLightReleasePoint = _fvwParent->m_pProxy->annotationClicked(docGlobalPos, selectText, t_strUUid);

            dApp->m_pAppInfo->setMousePressLocal(bIsHighLightReleasePoint, globalPos);
            if (bIsHighLightReleasePoint) {
                _fvwParent->__CloseFileNoteWidget();
                int nPage = _fvwParent->m_pProxy->pointInWhichPage(docGlobalPos);
                QString msgContent = t_strUUid.trimmed() + Constant::sQStringSep + QString::number(nPage);

                _fvwParent->slotDealWithMenu(MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, msgContent);
            }
        }
    } else if (bicon) {
        dApp->m_pAppInfo->setMousePressLocal(false, globalPos);
        _fvwParent->__CloseFileNoteWidget();

        int nPage = _fvwParent->m_pProxy->pointInWhichPage(docGlobalPos);
        QString strContent = t_strUUid.trimmed() + Constant::sQStringSep + QString::number(nPage) + Constant::sQStringSep +
                             QString::number(globalPos.x()) + Constant::sQStringSep +
                             QString::number(globalPos.y());

        QJsonObject obj;
        obj.insert("content", strContent);
        obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

        QJsonDocument doc(obj);

        notifyMsg(MSG_NOTE_PAGE_SHOW_NOTEWIDGET, doc.toJson(QJsonDocument::Compact));
    }

    m_bSelectOrMove = false;
}

void ProxyMouseMove::notifyMsg(const int &msgType, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
}
