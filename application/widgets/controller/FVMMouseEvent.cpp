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

#include "FVMMouseEvent.h"

#include <QMouseEvent>
#include <QDesktopServices>

#include "business/AppInfo.h"
#include "docview/docummentproxy.h"

#include "../FileViewWidget.h"
#include "../NoteTipWidget.h"

#include "gof/bridge/IHelper.h"

void FVMMouseEvent::mouseMoveEvent(QMouseEvent *event, DWidget *widget)
{
    if (widget == nullptr)
        return;

    FileViewWidget *fvw = qobject_cast<FileViewWidget *>(widget);

    //  处于幻灯片模式下
    if (dApp->m_pAppInfo->qGetCurShowState() == FILE_SLIDE) {
        emit fvw->sigShowPlayCtrl(true);
        return;
    }

    QPoint globalPos = event->globalPos();
    if (fvw->m_nCurrentHandelState == Handel_State) {  //   手型状态下， 按住鼠标左键 位置进行移动
        if (m_bSelectOrMove) {
            __FilePageMove(fvw, globalPos);
        }
    } else if (fvw->m_nCurrentHandelState == Magnifier_State) {  //  当前是放大镜状态
        __ShowMagnifier(fvw, globalPos);
    } else {
        if (m_bSelectOrMove) {  //  鼠标已经按下，　则选中所经过的文字
            __MouseSelectText(fvw, globalPos);
        } else {
            __OtherMouseMove(fvw, globalPos);
        }
    }
}


//  鼠标按住, 拖动页面
void FVMMouseEvent::__FilePageMove(FileViewWidget *fvw, const QPoint &endPos)
{
    QPoint mvPoint = m_pMoveStartPoint - endPos;
    int mvX = mvPoint.x();
    int mvY = mvPoint.y();

    fvw->m_pProxy->pageMove(mvX, mvY);

    fvw->__SetCursor(QCursor(Qt::OpenHandCursor));

    m_pMoveStartPoint = endPos;
}

//  显示放大镜 数据
void FVMMouseEvent::__ShowMagnifier(FileViewWidget *fvw, const QPoint &clickPoint)
{
    QPoint docGlobalPos = fvw->m_pProxy->global2RelativePoint(clickPoint);

    fvw->m_pProxy->showMagnifier(docGlobalPos);
}

//  选中文本
void FVMMouseEvent::__MouseSelectText(FileViewWidget *fvw, const QPoint &clickPoint)
{
    QPoint docGlobalPos = fvw->m_pProxy->global2RelativePoint(clickPoint);

    fvw->m_pEndSelectPoint = docGlobalPos;
    fvw->m_pProxy->mouseSelectText(fvw->m_pStartPoint, fvw->m_pEndSelectPoint);
}

void FVMMouseEvent::__ShowNoteTipWidget(FileViewWidget *fvw, const QString &sText)
{
    NoteTipWidget *tipWidget = fvw->findChild<NoteTipWidget *>();
    if (tipWidget == nullptr) {
        tipWidget = new NoteTipWidget(fvw);
    }
    tipWidget->setTipContent(sText);
    QPoint showRealPos(QCursor::pos().x(), QCursor::pos().y() + 10);
    tipWidget->move(showRealPos);
    tipWidget->show();
}

//  显示页面注释 提示界面
void FVMMouseEvent::__ShowPageNoteWidget(FileViewWidget *fvw, const QPoint &docPos)
{
    QString sText = "", sUuid = "";
    bool isOk = fvw->m_pProxy->iconAnnotationClicked(docPos, sText, sUuid);
    if (isOk) {
        if (sText != "") {
            __ShowNoteTipWidget(fvw, sText);
        }
    }
}

//  显示 高亮注释 提示界面
void FVMMouseEvent::__ShowFileNoteWidget(FileViewWidget *fvw, const QPoint &docPos)
{
    QString selectText, t_strUUid;
    bool bIsHighLightReleasePoint = fvw->m_pProxy->annotationClicked(docPos, selectText, t_strUUid);
    if (bIsHighLightReleasePoint) {
        int nPage = fvw->m_pProxy->pointInWhichPage(docPos);
        QString sText = "";
        fvw->m_pProxy->getAnnotationText(t_strUUid, sText, nPage);
        if (sText != "") {
            __ShowNoteTipWidget(fvw, sText);
        }
    }
}

//  关闭 注释tip 对话框
void FVMMouseEvent::__CloseFileNoteWidget(FileViewWidget *fvw)
{
    NoteTipWidget *tipWidget = fvw->findChild<NoteTipWidget *>();
    if (tipWidget && tipWidget->isVisible()) {
        tipWidget->close();
    }
}

//  其余 鼠标移动
void FVMMouseEvent::__OtherMouseMove(FileViewWidget *fvw, const QPoint &globalPos)
{
    QPoint docGlobalPos = fvw->m_pProxy->global2RelativePoint(globalPos);

    //  首先判断文档划过属性
    auto pLink = fvw->m_pProxy->mouseBeOverLink(docGlobalPos);
    if (pLink) {
        fvw->__SetCursor(QCursor(Qt::PointingHandCursor));
    } else {
        //  先判断是否是页面注释图标
        if (fvw->m_pProxy->mouseovericonAnnotation(docGlobalPos)) {
            __ShowPageNoteWidget(fvw, docGlobalPos);
        } else if (fvw->m_pProxy->mouseBeOverText(docGlobalPos)) {
            m_bIsHandleSelect = true;

            fvw->__SetCursor(QCursor(Qt::IBeamCursor));

            __ShowFileNoteWidget(fvw, docGlobalPos);
        } else {
            m_bIsHandleSelect = false;

            fvw->__SetCursor(QCursor(Qt::ArrowCursor));

            __CloseFileNoteWidget(fvw);
        }
    }
}


void FVMMouseEvent::mousePressEvent(QMouseEvent *event, DWidget *widget)
{
    if (widget == nullptr)
        return;

    FileViewWidget *fvw = qobject_cast<FileViewWidget *>(widget);

    Qt::MouseButton nBtn = event->button();
    if (nBtn == Qt::RightButton) {  //  右键处理
        //  处于幻灯片模式下
        if (dApp->m_pAppInfo->qGetCurShowState() == FILE_SLIDE) {
            fvw->notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_esc);
            return;
        }

        //  放大镜状态，
        if (fvw->m_nCurrentHandelState == Magnifier_State) {
            fvw->notifyMsg(MSG_MAGNIFYING_CANCEL);
            return;
        }
    } else if (nBtn == Qt::LeftButton) { // 左键处理
        //  幻灯片模式下, 左键单击 不作任何处理
        if (dApp->m_pAppInfo->qGetCurShowState() == FILE_SLIDE)
            return;

        QPoint globalPos = event->globalPos();
        //  当前状态是 手, 先 拖动, 之后 在是否是链接之类
        if (fvw->m_nCurrentHandelState == NOTE_ADD_State) {
            __AddIconAnnotation(fvw, globalPos);
        } else if (fvw->m_nCurrentHandelState == Handel_State) {
            __HandlClicked(globalPos);
        } else {
            __OtherMousePress(fvw, globalPos);
        }
    }
}

//  页面添加注释图标
void FVMMouseEvent::__AddIconAnnotation(FileViewWidget *fvw, const QPoint &globalPos)
{
    QPoint docGlobalPos = fvw->m_pProxy->global2RelativePoint(globalPos);

    //  添加之前先判断, 点击处是否 注释图标
    bool bIsIcon = fvw->m_pProxy->mouseovericonAnnotation(docGlobalPos);
    if (bIsIcon) {
        QString sContent = "", sUuid = "";
        if (fvw->m_pProxy->iconAnnotationClicked(docGlobalPos, sContent, sUuid)) {

            qDebug() << __FUNCTION__ << "            " << sContent << "      " << sUuid;
        }
    } else {
        QString sUuid = fvw->m_pProxy->addIconAnnotation(docGlobalPos);        //  添加注释图标成功
        if (sUuid != "") {
            int nClickPage = fvw->m_pProxy->pointInWhichPage(docGlobalPos);
            QString strContent = sUuid.trimmed() + Constant::sQStringSep +
                                 QString::number(nClickPage) + Constant::sQStringSep +
                                 QString::number(globalPos.x()) + Constant::sQStringSep +
                                 QString::number(globalPos.y());
            fvw->m_nCurrentHandelState = Default_State;
            emit fvw->signalDealWithData(MSG_NOTE_PAGE_SHOW_NOTEWIDGET, strContent);
        } else {
            qWarning() << __FUNCTION__ << "          " << sUuid;;
        }
    }
}

void FVMMouseEvent::__HandlClicked(const QPoint &globalPos)
{
    m_pMoveStartPoint = globalPos;  //  变成手，　需要的是　相对坐标
    m_bSelectOrMove = true;
}

//  点击 页面能够跳转
void FVMMouseEvent::__ClickPageLink(Page::Link *pLink, FileViewWidget *fvw)
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

void FVMMouseEvent::__OtherMousePress(FileViewWidget *fvw, const QPoint &globalPos)
{
    QPoint docGlobalPos = fvw->m_pProxy->global2RelativePoint(globalPos);

    //  点击的时候　先判断　点击处　　是否有链接之类
    auto pLink = fvw->m_pProxy->mouseBeOverLink(docGlobalPos);
    if (pLink) {
        __ClickPageLink(pLink, fvw);
    } else {
        if (fvw->m_nCurrentHandelState == Default_State) {
            fvw->m_pProxy->mouseSelectTextClear();  //  清除之前选中的文字高亮

            __CloseFileNoteWidget(fvw);

            if (m_bIsHandleSelect) {
                m_bSelectOrMove = true;
                fvw->m_pStartPoint = docGlobalPos;
                fvw->m_pEndSelectPoint = fvw->m_pStartPoint;
            }
        }
    }
}


void FVMMouseEvent::mouseReleaseEvent(QMouseEvent *event, DWidget *widget)
{
    //  幻灯片模式下, 左键单击 不作任何处理
    if (dApp->m_pAppInfo->qGetCurShowState() == FILE_SLIDE)
        return;

    Qt::MouseButton nBtn = event->button();

    FileViewWidget *fvw = qobject_cast<FileViewWidget *>(widget);
    QPoint globalPos = event->globalPos();
    QPoint docGlobalPos = fvw->m_pProxy->global2RelativePoint(globalPos);
    QString selectText, t_strUUid;
    bool bicon = fvw->m_pProxy->iconAnnotationClicked(docGlobalPos, selectText, t_strUUid);
    //  放大镜状态， 右键则取消放大镜 并且 直接返回
    if (m_bSelectOrMove && !bicon) {
        //判断鼠标左键松开的位置有没有高亮

        dApp->m_pAppInfo->setMousePressLocal(false, globalPos);
        //添加其实结束point是否为同一个，不是同一个说明不是点击可能是选择文字
        if (nBtn == Qt::LeftButton && docGlobalPos == fvw->m_pStartPoint) {
            // 判断鼠标点击的地方是否有高亮
            bool bIsHighLightReleasePoint = fvw->m_pProxy->annotationClicked(docGlobalPos, selectText, t_strUUid);

            dApp->m_pAppInfo->setMousePressLocal(bIsHighLightReleasePoint, globalPos);
            if (bIsHighLightReleasePoint) {
                __CloseFileNoteWidget(fvw);
                int nPage = fvw->m_pProxy->pointInWhichPage(docGlobalPos);
                QString t_strContant = t_strUUid.trimmed() + Constant::sQStringSep + QString::number(nPage);
                emit fvw->signalDealWithData(MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, t_strContant);
            }
        }
    } else if (bicon) {
        __CloseFileNoteWidget(fvw);
        int nPage = fvw->m_pProxy->pointInWhichPage(docGlobalPos);
        QString t_strContant = t_strUUid.trimmed() + Constant::sQStringSep + QString::number(nPage) + Constant::sQStringSep +
                               QString::number(globalPos.x()) + Constant::sQStringSep +
                               QString::number(globalPos.y());
        emit fvw->signalDealWithData(MSG_NOTE_PAGE_SHOW_NOTEWIDGET, t_strContant);
    }

    m_bSelectOrMove = false;
}
