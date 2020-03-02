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
#include "FileViewWidget.h"

#include <QClipboard>
#include <QGridLayout>
#include <QDesktopServices>
#include <DDialog>

#include <QFileInfo>

#include "MsgModel.h"

#include "business/FileFormatHelper.h"

#include "docview/commonstruct.h"
#include "docview/docummentproxy.h"
#include "controller/FVMMouseEvent.h"
#include "business/FileDataManager.h"
#include "menu/DefaultOperationMenu.h"
#include "menu/TextOperationMenu.h"
#include "utils/PublicFunction.h"
#include "business/PrintManager.h"

#include "business/bridge/IHelper.h"

#include "main/MainTabWidgetEx.h"

FileViewWidget::FileViewWidget(CustomWidget *parent)
    : CustomWidget("FileViewWidget", parent)
    , m_operatemenu(nullptr)
{
    m_pMsgList = { MSG_MAGNIFYING, MSG_HANDLESHAPE,
                   MSG_FILE_ROTATE,
                   MSG_NOTE_ADD_CONTENT, MSG_NOTE_PAGE_ADD,
                   MSG_VIEWCHANGE_DOUBLE_SHOW, MSG_VIEWCHANGE_ROTATE, MSG_FILE_SCALE, MSG_VIEWCHANGE_FIT
                 };

    m_pKeyMsgList = {KeyStr::g_ctrl_p, KeyStr::g_ctrl_l, KeyStr::g_ctrl_i};

    setMouseTracking(true);  //  接受 鼠标滑动事件

    initWidget();
    initConnections();

    dApp->m_pModelService->addObserver(this);
}

FileViewWidget::~FileViewWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

void FileViewWidget::initWidget()
{
}

//  鼠标移动
void FileViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    g_FVMMouseEvent::getInstance()->mouseMoveEvent(event, this);
}

//  鼠标左键 按下
void FileViewWidget::mousePressEvent(QMouseEvent *event)
{
    g_FVMMouseEvent::getInstance()->mousePressEvent(event, this);
}

//  鼠标松开
void FileViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    g_FVMMouseEvent::getInstance()->mouseReleaseEvent(event, this);
}

//  文档 显示区域 大小变化
void FileViewWidget::resizeEvent(QResizeEvent *event)
{
    onSetWidgetAdapt();

    CustomWidget::resizeEvent(event);
}

void FileViewWidget::wheelEvent(QWheelEvent *event)
{
    CustomWidget::wheelEvent(event);
    if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
        if (event->delta() > 0) {
            notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_larger);
        } else {
            notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_smaller);
        }
    }
}

void FileViewWidget::slotDealWithData(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_MAGNIFYING:            //  放大镜信号
        onMagnifying(msgContent);
        break;
    case MSG_FILE_ROTATE:           //  文档旋转了
        onSetWidgetAdapt();
        break;
    case MSG_NOTE_ADD_CONTENT:                   //  添加注释
        onFileAddNote(msgContent);
        break;
    case MSG_NOTE_PAGE_ADD:                     //  2020.2.18   wzx add
        __SetPageAddIconState(msgContent);
        break;
    default:
        break;
    }
}

void FileViewWidget::slotDealWithKeyMsg(const QString &msgContent)
{
    if (!m_pProxy)
        return;

    if (msgContent == KeyStr::g_ctrl_p) {
//        onPrintFile();
    } else if (msgContent == KeyStr::g_ctrl_l) {
        onFileAddAnnotation();
    } else if (msgContent == KeyStr::g_ctrl_i) {
        QString selectText;
        if (m_pProxy->getSelectTextString(selectText))
            onFileAddNote();
        else {
            notifyMsg(CENTRAL_SHOW_TIP, tr("Please select the text"));
        }
    }
}

//  弹出 自定义 菜单
void FileViewWidget::slotCustomContextMenuRequested(const QPoint &point)
{
    if (!m_pProxy)
        return;

    //  处于幻灯片模式下
    if (dApp->m_pAppInfo->qGetCurShowState() == FILE_SLIDE)
        return;

    //  放大镜状态， 直接返回
    if (m_nCurrentHandelState == Magnifier_State)
        return;

    //  手型状态， 直接返回
    if (m_nCurrentHandelState == Handel_State)
        return;

    QString sSelectText = "";
    m_pProxy->getSelectTextString(sSelectText);  //  选择　当前选中下面是否有文字

    QPoint tempPoint = this->mapToGlobal(point);
    QPoint pRightClickPoint = m_pProxy->global2RelativePoint(tempPoint);

    //  右键鼠标点 是否有高亮区域
    QString sAnnotationText = "", struuid = "";
    bool bIsHighLight = m_pProxy->annotationClicked(pRightClickPoint, sAnnotationText, struuid);
    bool bicon = m_pProxy->iconAnnotationClicked(pRightClickPoint, sAnnotationText, struuid);
    int nPage = m_pProxy->pointInWhichPage(pRightClickPoint);
    if (nullptr == m_operatemenu) {
        m_operatemenu = new TextOperationMenu(this);
    }
    if ((sSelectText != "" || bIsHighLight) && !bicon) { //  选中区域 有文字, 弹出 文字操作菜单
        //  需要　区别　当前选中的区域，　弹出　不一样的　菜单选项
        m_operatemenu->setClickPoint(pRightClickPoint);
        m_operatemenu->setPStartPoint(m_pStartPoint);
        m_operatemenu->setPEndPoint(m_pEndSelectPoint);
        m_operatemenu->setClickPage(nPage);
        m_operatemenu->setType(NOTE_HIGHLIGHT);
        dApp->m_pAppInfo->setMousePressLocal(bIsHighLight, tempPoint);

        m_operatemenu->execMenu(tempPoint, bIsHighLight, sSelectText, struuid);
    } else if (bicon) {
        if (nullptr == m_operatemenu) {
            m_operatemenu = new TextOperationMenu(this);
            connect(m_operatemenu, SIGNAL(sigDealWithData(const int &, const QString &)), this, SIGNAL(signalDealWithData(const int &, const QString &)));
        }

        m_operatemenu->setClickPoint(pRightClickPoint);
        m_operatemenu->setPStartPoint(m_pStartPoint);
        m_operatemenu->setPEndPoint(m_pEndSelectPoint);
        m_operatemenu->setClickPage(nPage);
        m_operatemenu->setType(NOTE_ICON);
        dApp->m_pAppInfo->setMousePressLocal(bIsHighLight, tempPoint);
        m_operatemenu->execMenu(tempPoint, bicon, sSelectText, struuid);
    } else {  //  否则弹出 文档操作菜单
        if (nullptr == m_pDefaultMenu) {
            m_pDefaultMenu = new DefaultOperationMenu(this);
            connect(m_pDefaultMenu, SIGNAL(sigDealWithData(const int &, const QString &)), this, SIGNAL(signalDealWithData(const int &, const QString &)));
        }
        m_pDefaultMenu->setClickpoint(pRightClickPoint);
        m_pDefaultMenu->execMenu(tempPoint, nPage);

    }
}

//  放大镜　控制
void FileViewWidget::onMagnifying(const QString &data)
{
    if (!m_pProxy)
        return;

    int nRes = data.toInt();
    if (nRes == 1) {
        m_pProxy->mouseSelectTextClear();  //  清除之前选中的文字高亮

        m_nCurrentHandelState = Magnifier_State;
        __SetCursor(Qt::BlankCursor);
    } else {
        if (m_nCurrentHandelState == Magnifier_State) {  //  是 放大镜模式 才取消

            m_nCurrentHandelState = Default_State;
            __SetCursor(Qt::ArrowCursor);

            m_pProxy->closeMagnifier();
        }
    }
}

//  手势控制
void FileViewWidget::onSetHandShape(const QString &data)
{
    if (!m_pProxy)
        return;

    //  手型 切换 也需要将之前选中的文字清除 选中样式
    m_pProxy->mouseSelectTextClear();
    int nRes = data.toInt();
    if (nRes == 1) {  //  手形
        m_nCurrentHandelState = Handel_State;
        __SetCursor(Qt::OpenHandCursor);
    } else {
        m_nCurrentHandelState = Default_State;
        __SetCursor(Qt::ArrowCursor);
    }
}

//  添加高亮颜色  快捷键
void FileViewWidget::onFileAddAnnotation()
{
    //  处于幻灯片模式下
    if (dApp->m_pAppInfo->qGetCurShowState() == FILE_SLIDE)
        return;

    //  放大镜状态， 直接返回
    if (m_nCurrentHandelState == Magnifier_State)
        return;

    //  手型状态， 直接返回
    if (m_nCurrentHandelState == Handel_State)
        return;

    int nSx = m_pStartPoint.x();
    int nSy = m_pStartPoint.y();

    int nEx = m_pEndSelectPoint.x();
    int nEy = m_pEndSelectPoint.y();

    if (nSx == nEx && nSy == nEy) {
        notifyMsg(CENTRAL_SHOW_TIP, tr("Please select the text"));
        return;
    }

    if (!m_pProxy)
        return;

    QString selectText = "";
    m_pProxy->getSelectTextString(selectText);
    if (selectText != "") {
        QString sContent = QString::number(nSx) + Constant::sQStringSep +
                           QString::number(nSy) + Constant::sQStringSep +
                           QString::number(nEx) + Constant::sQStringSep +
                           QString::number(nEy);

        dApp->m_pHelper->qDealWithData(MSG_NOTE_ADD_HIGHLIGHT_COLOR, sContent);
    } else {
        notifyMsg(CENTRAL_SHOW_TIP, tr("Please select the text"));
    }
}

//  添加注释
void FileViewWidget::onFileAddNote(const QString &msgContent)
{
    QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (contentList.size() == 3) {
        QString sPath = contentList.at(0);
        if (sPath == m_strPath) {
            QString sNote = contentList.at(1);
            QString sPage = contentList.at(2);

            int nSx = m_pStartPoint.x();
            int nSy = m_pStartPoint.y();

            int nEx = m_pEndSelectPoint.x();
            int nEy = m_pEndSelectPoint.y();

            QString sContent = QString::number(nSx) + Constant::sQStringSep +
                               QString::number(nSy) + Constant::sQStringSep +
                               QString::number(nEx) + Constant::sQStringSep +
                               QString::number(nEy) + Constant::sQStringSep +
                               sNote + Constant::sQStringSep +
                               sPage;

            dApp->m_pHelper->qDealWithData(MSG_NOTE_ADD_HIGHLIGHT_NOTE, sContent);
        }
    }
}

void FileViewWidget::__SetPageAddIconState(const QString &sPath)
{
    if (m_strPath == sPath) {
        m_nCurrentHandelState = NOTE_ADD_State;
    }
}

void FileViewWidget::onFileAddNote()
{
    if (!m_pProxy)
        return;

    //  处于幻灯片模式下
    if (dApp->m_pAppInfo->qGetCurShowState() == FILE_SLIDE)
        return;

    //  放大镜状态， 直接返回
    if (m_nCurrentHandelState == Magnifier_State)
        return;

    //  手型状态， 直接返回
    if (m_nCurrentHandelState == Handel_State)
        return;

    int nSx = m_pStartPoint.x();
    int nSy = m_pStartPoint.y();

    int nEx = m_pEndSelectPoint.x();
    int nEy = m_pEndSelectPoint.y();

    if ((nSx == nEx && nSy == nEy)) {
        notifyMsg(CENTRAL_SHOW_TIP, tr("Please select the text"));
        return;
    }

    int nPage = m_pProxy->pointInWhichPage(m_pEndSelectPoint);
    QString msgContent = QString("%1").arg(nPage) + Constant::sQStringSep +
                         QString("%1").arg(m_pEndSelectPoint.x()) + Constant::sQStringSep +
                         QString("%1").arg(m_pEndSelectPoint.y());
    emit signalDealWithData(MSG_OPERATION_TEXT_ADD_ANNOTATION, msgContent);
}

//  设置鼠标状态
void FileViewWidget::__SetCursor(const QCursor &cs)
{
    const QCursor ss = cursor();    //  当前鼠标状态
    if (ss != cs) {
        setCursor(cs);
    }
}

void FileViewWidget::OnSetViewChange(const QString &msgContent)
{
    m_nDoubleShow = msgContent.toInt();
    setScaleRotateViewModeAndShow();
}

void FileViewWidget::setScaleRotateViewModeAndShow()
{
    if (m_pProxy) {
        double dScale = m_nScale / 100.0;
        ViewMode_EM em = m_nDoubleShow ? ViewMode_FacingPage : ViewMode_SinglePage;
        m_pProxy->setScaleRotateViewModeAndShow(dScale, static_cast<RotateType_EM>(m_rotateType), em);
    }
}

bool FileViewWidget::OpenFilePath(const QString &sPath)
{
    m_strPath = sPath;
    //  实际文档类  唯一实例化设置 父窗口
    QString m_strProcUuid = DocummentProxy::CreateInstance(this);

    m_pProxy = DocummentProxy::instance(m_strProcUuid);
    if (m_pProxy) {
        connect(m_pProxy, SIGNAL(signal_bookMarkStateChange(int, bool)), SLOT(slotBookMarkStateChange(int, bool)));
        connect(m_pProxy, SIGNAL(signal_pageChange(int)), SLOT(slotDocFilePageChanged(int)));
        connect(m_pProxy, SIGNAL(signal_openResult(bool)), SLOT(SlotDocFileOpenResult(bool)));

        QFileInfo info(sPath);

        QString sCompleteSuffix = info.completeSuffix();
        DocType_EM nCurDocType = FFH::setCurDocuType(sCompleteSuffix);

        //从数据库中获取文件的字号信息
        dApp->m_pDBService->qSelectData(sPath, DB_HISTROY);
        FileDataModel fdm = dApp->m_pDBService->getHistroyData(sPath);

        dApp->m_pDataManager->qSetFileData(sPath, fdm);

        int curPage = fdm.qGetData(CurPage);
        int iscale = fdm.qGetData(Scale);          // 缩放
        int doubPage = fdm.qGetData(DoubleShow);     // 是否是双页
        int rotate = fdm.qGetData(Rotate);         // 文档旋转角度(0,1,2,3,4)

        iscale = (iscale > 500 ? 500 : iscale) <= 0 ? 100 : iscale;
        double scale = iscale / 100.0;
        RotateType_EM rotatetype = static_cast<RotateType_EM>(rotate);
        ViewMode_EM viewmode = static_cast<ViewMode_EM>(doubPage);

        bool rl = m_pProxy->openFile(nCurDocType, sPath, curPage, rotatetype, scale, viewmode);
        if (rl) {
            MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
            if (pMtwe) {
                pMtwe->InsertPathProxy(sPath, m_pProxy);       //  存储 filePath 对应的 proxy
            }

            return rl;
        }
    }
    return false;
}

void FileViewWidget::OnSetViewScale(const QString &msgConent)
{
    m_nScale = msgConent.toInt();

    setScaleRotateViewModeAndShow();
}

void FileViewWidget::OnSetViewRotate(const QString &msgConent)
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

    setScaleRotateViewModeAndShow();
}

void FileViewWidget::OnSetViewHit(const QString &msgContent)
{
    if (msgContent == "1") {
        m_nAdapteState = WIDGET_State;
    } else if (msgContent == "10") {
        m_nAdapteState = HEIGHT_State;
    } else {
        m_nAdapteState = Default_State;
    }
    onSetWidgetAdapt();
}

//  文档书签状态改变
void FileViewWidget::slotBookMarkStateChange(int nPage, bool bState)
{
    if (!bState) {
        emit signalDealWithData(MSG_OPERATION_DELETE_BOOKMARK, QString("%1").arg(nPage));
    } else {
        emit signalDealWithData(MSG_OPERATION_ADD_BOOKMARK, QString("%1").arg(nPage));
    }
}

//  文档页变化了
void FileViewWidget::slotDocFilePageChanged(int page)
{
    emit signalDealWithData(MSG_FILE_PAGE_CHANGE, QString::number(page));
}

//  打开文档结果
void FileViewWidget::SlotDocFileOpenResult(bool openresult)
{
    //  通知 其他窗口， 打开文件成功了！！！
    if (openresult) {
        dApp->m_pDBService->qSelectData(m_strPath, DB_BOOKMARK);

        emit signalDealWithData(MSG_OPERATION_OPEN_FILE_OK, "");

        FileDataModel fdm = dApp->m_pDataManager->qGetFileData(m_strPath);      //  打开成功之后 获取之前数据
        m_nScale = fdm.qGetData(Scale);
        m_nDoubleShow = fdm.qGetData(DoubleShow);
        m_rotateType = fdm.qGetData(Rotate);

        m_nAdapteState = fdm.qGetData(Fit);
        onSetWidgetAdapt();
    } else {
        notifyMsg(MSG_OPERATION_OPEN_FILE_FAIL, tr("Please check if the file is damaged"));
    }
}

//  信号槽　初始化
void FileViewWidget::initConnections()
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotCustomContextMenuRequested(const QPoint &)));

    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)), SLOT(slotDealWithData(const int &, const QString &)));
    connect(this, SIGNAL(sigDealWithKeyMsg(const QString &)), SLOT(slotDealWithKeyMsg(const QString &)));
}

//  设置　窗口　自适应　宽＼高　度
void FileViewWidget::onSetWidgetAdapt()
{
    if (m_nAdapteState != Default_State) {
        if (!m_pProxy)
            return;

        double nScale = 0.0;
        if (m_nAdapteState == WIDGET_State) {
            int nWidth = this->width();
            nScale = m_pProxy->adaptWidthAndShow(nWidth);
        } else if (m_nAdapteState == HEIGHT_State) {
            int nHeight = this->height();
            nScale = m_pProxy->adaptHeightAndShow(nHeight);
        }

//    if (nScale != 0.0) {
//        notifyMsg(MSG_FILE_SCALE, QString::number(nScale));
//    }
    }
}
//  消息 数据 处理
int FileViewWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
    } else if (msgType == MSG_NOTIFY_KEY_MSG && this->isVisible()) {
        if (m_pKeyMsgList.contains(msgContent)) {
            emit sigDealWithKeyMsg(msgContent);
            return ConstantMsg::g_effective_res;
        }
    }

    return 0;
}

int FileViewWidget::qDealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_VIEWCHANGE_DOUBLE_SHOW) {
        OnSetViewChange(msgContent);
    } else if (msgType == MSG_VIEWCHANGE_ROTATE) {
        OnSetViewRotate(msgContent);
    } else if (msgType == MSG_FILE_SCALE) {
        OnSetViewScale(msgContent);
    } else if (msgType == MSG_VIEWCHANGE_FIT) {
        OnSetViewHit(msgContent);
    } else if (msgType == MSG_HANDLESHAPE) {
        onSetHandShape(msgContent);
    }

    bool rl = m_pMsgList.contains(msgType);
    if (rl) {
        return MSG_OK;
    }

    return MSG_NO_OK;
}
