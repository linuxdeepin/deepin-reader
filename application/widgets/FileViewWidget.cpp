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

#include "MsgModel.h"

#include "business/DocummentFileHelper.h"
#include "business/AnnotationHelper.h"
#include "docview/docummentproxy.h"
#include "controller/AppSetting.h"
#include "controller/FVMMouseEvent.h"
#include "controller/FileDataManager.h"
#include "menu/DefaultOperationMenu.h"
#include "menu/TextOperationMenu.h"
#include "utils/PublicFunction.h"
#include "business/PrintManager.h"

FileViewWidget::FileViewWidget(CustomWidget *parent)
    : CustomWidget("FileViewWidget", parent)
    , m_operatemenu(nullptr)
{
    m_pMsgList = { MSG_MAGNIFYING, MSG_HANDLESHAPE, MSG_VIEWCHANGE_FIT,
                   MSG_FILE_ROTATE,
                   MSG_NOTE_ADD_CONTENT, MSG_NOTE_PAGE_ADD,
                   MSG_EXIT_SAVE_FILE
                 };

    m_pKeyMsgList = {KeyStr::g_ctrl_p, KeyStr::g_ctrl_l, KeyStr::g_ctrl_i};

    setMouseTracking(true);  //  接受 鼠标滑动事件

    initWidget();
    initConnections();

    dApp->m_pModelService->addObserver(this);
}

FileViewWidget::~FileViewWidget()
{
    dApp->m_pModelService->addObserver(this);
}

void FileViewWidget::initWidget()
{
    //  实际文档类  唯一实例化设置 父窗口
    m_strProcUuid = DocummentProxy::CreateInstance(this);

    auto m_pDocummentProxy = DocummentProxy::instance(m_strProcUuid);
    if (m_pDocummentProxy) {
        connect(m_pDocummentProxy, SIGNAL(signal_bookMarkStateChange(int, bool)), SLOT(slotBookMarkStateChange(int, bool)));
        connect(m_pDocummentProxy, SIGNAL(signal_pageChange(int)), SLOT(slotDocFilePageChanged(int)));
        connect(m_pDocummentProxy, SIGNAL(signal_openResult(bool)), SLOT(SlotDocFileOpenResult(bool)));
    }
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
    case MSG_HANDLESHAPE:           //  手势 信号
        onSetHandShape(msgContent);
        break;
    case MSG_VIEWCHANGE_FIT:    //  自适应
        if (msgContent == "1") {
            m_nAdapteState = HEIGHT_State;
            onSetWidgetAdapt();
        } else if (msgContent == "10") {
            m_nAdapteState = WIDGET_State;
            onSetWidgetAdapt();
        } else {
            m_nAdapteState = Default_State;
        }
        break;
    case MSG_FILE_ROTATE:           //  文档旋转了
        onSetWidgetAdapt();
        break;
    case MSG_NOTE_ADD_CONTENT:                   //  添加注释
        onFileAddNote(msgContent);
        break;
    case MSG_NOTE_PAGE_ADD:                     //  2020.2.18   wzx add
        __SetPageAddIconState();
        break;
    default:
        break;
    }
}

void FileViewWidget::slotDealWithKeyMsg(const QString &msgContent)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
        return;

    if (msgContent == KeyStr::g_ctrl_p) {
        onPrintFile();
    } else if (msgContent == KeyStr::g_ctrl_l) {
        onFileAddAnnotation();
    } else if (msgContent == KeyStr::g_ctrl_i) {
        QString selectText;
        if (_proxy->getSelectTextString(selectText))
            onFileAddNote();
        else {
            notifyMsg(CENTRAL_SHOW_TIP, tr("Please select the text"));
        }
    }
}

//  弹出 自定义 菜单
void FileViewWidget::slotCustomContextMenuRequested(const QPoint &point)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
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
    _proxy->getSelectTextString(sSelectText);  //  选择　当前选中下面是否有文字

    QPoint tempPoint = this->mapToGlobal(point);
    QPoint pRightClickPoint = _proxy->global2RelativePoint(tempPoint);

    //  右键鼠标点 是否有高亮区域
    QString sAnnotationText = "", struuid = "";
    bool bIsHighLight = _proxy->annotationClicked(pRightClickPoint, sAnnotationText, struuid);
    bool bicon = _proxy->iconAnnotationClicked(pRightClickPoint, sAnnotationText, struuid);
    int nPage = _proxy->pointInWhichPage(pRightClickPoint);
    if ((sSelectText != "" || bIsHighLight) && !bicon) { //  选中区域 有文字, 弹出 文字操作菜单
        //  需要　区别　当前选中的区域，　弹出　不一样的　菜单选项
        if (nullptr == m_operatemenu) {
            m_operatemenu = new TextOperationMenu(this);
        }
        m_operatemenu->setClickPoint(pRightClickPoint);
        m_operatemenu->setPStartPoint(m_pStartPoint);
        m_operatemenu->setPEndPoint(m_pEndSelectPoint);
        m_operatemenu->setClickPage(nPage);

        dApp->m_pAppInfo->setMousePressLocal(bIsHighLight, tempPoint);

        m_operatemenu->execMenu(tempPoint, bIsHighLight, sSelectText, struuid);
    } else if (bicon) {
        if (nullptr == m_operatemenu) {
            m_operatemenu = new TextOperationMenu(this);
        }
        m_operatemenu->setClickPoint(pRightClickPoint);
        m_operatemenu->setPStartPoint(m_pStartPoint);
        m_operatemenu->setPEndPoint(m_pEndSelectPoint);
        m_operatemenu->setClickPage(nPage);

        dApp->m_pAppInfo->setMousePressLocal(bIsHighLight, tempPoint);
        m_operatemenu->execMenu(tempPoint, bicon, sSelectText, struuid);
    } else {  //  否则弹出 文档操作菜单
        auto menu = new DefaultOperationMenu(this);
        menu->execMenu(tempPoint, nPage);
    }
}

//  放大镜　控制
void FileViewWidget::onMagnifying(const QString &data)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
        return;

    int nRes = data.toInt();
    if (nRes == 1) {
        _proxy->mouseSelectTextClear();  //  清除之前选中的文字高亮

        m_nCurrentHandelState = Magnifier_State;
        __SetCursor(Qt::BlankCursor);
    } else {
        if (m_nCurrentHandelState == Magnifier_State) {  //  是 放大镜模式 才取消

            m_nCurrentHandelState = Default_State;
            __SetCursor(Qt::ArrowCursor);

            _proxy->closeMagnifier();
        }
    }
}

//  手势控制
void FileViewWidget::onSetHandShape(const QString &data)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
        return;

    //  手型 切换 也需要将之前选中的文字清除 选中样式
    _proxy->mouseSelectTextClear();
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

    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
        return;

    QString selectText = "";
    _proxy->getSelectTextString(selectText);
    if (selectText != "") {
        QString sContent = QString::number(nSx) + Constant::sQStringSep +
                           QString::number(nSy) + Constant::sQStringSep +
                           QString::number(nEx) + Constant::sQStringSep +
                           QString::number(nEy);

        notifyMsg(MSG_NOTE_ADD_HIGHLIGHT_COLOR, sContent);
    } else {
        notifyMsg(CENTRAL_SHOW_TIP, tr("Please select the text"));
    }
}

//  添加注释
void FileViewWidget::onFileAddNote(const QString &msgContent)
{
    QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (contentList.size() == 2) {
        QString sNote = contentList.at(0);
        QString sPage = contentList.at(1);

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

        notifyMsg(MSG_NOTE_ADD_HIGHLIGHT_NOTE, sContent);
    }
}

void FileViewWidget::__SetPageAddIconState()
{
    m_nCurrentHandelState = NOTE_ADD_State;
}

void FileViewWidget::onFileAddNote()
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
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

    int nPage = _proxy->pointInWhichPage(m_pEndSelectPoint);
    QString msgContent = QString("%1").arg(nPage) + Constant::sQStringSep +
                         QString("%1").arg(m_pEndSelectPoint.x()) + Constant::sQStringSep +
                         QString("%1").arg(m_pEndSelectPoint.y());
    notifyMsg(MSG_OPERATION_TEXT_ADD_ANNOTATION, msgContent);
}

//  设置鼠标状态
void FileViewWidget::__SetCursor(const QCursor &cs)
{
    const QCursor ss = cursor();    //  当前鼠标状态
    if (ss != cs) {
        setCursor(cs);
    }
}

void FileViewWidget::OnSetViewChange()
{
    auto _pProxy = DocummentProxy::instance(m_strProcUuid);
//    _pProxy->setScaleRotateViewModeAndShow(0, 0, m_bDoubleShow);
    qDebug() <<  __FUNCTION__  << "      " << m_bDoubleShow;
}

//  文档书签状态改变
void FileViewWidget::slotBookMarkStateChange(int nPage, bool bState)
{
    if (!bState) {
        sendMsg(MSG_OPERATION_DELETE_BOOKMARK, QString("%1").arg(nPage));
    } else {
        sendMsg(MSG_OPERATION_ADD_BOOKMARK, QString("%1").arg(nPage));
    }
}

//  文档页变化了
void FileViewWidget::slotDocFilePageChanged(int page)
{
    notifyMsg(MSG_FILE_PAGE_CHANGE, QString("%1").arg(page));
}

//  打开文档结果
void FileViewWidget::SlotDocFileOpenResult(bool openresult)
{
    //  通知 其他窗口， 打开文件成功了！！！
    if (openresult) {
        dApp->m_pDBService->qSelectData(m_strPath, DB_BOOKMARK);
        dApp->m_pDataManager->qInsertFileChange(m_strPath, false);
        dApp->m_pDataManager->qInsertFileOpen(m_strPath, true);

        MsgModel mm;
        mm.setMsgType(MSG_OPERATION_OPEN_FILE_OK);
        mm.setPath(m_strPath);

        notifyMsg(E_DOCPROXY_MSG_TYPE, mm.toJson());
    } else {
        notifyMsg(MSG_OPERATION_OPEN_FILE_FAIL, tr("Please check if the file is damaged"));
    }
}

void FileViewWidget::SlotCloseFile(const QString &sPath)
{
    emit sigClosetab(m_strProcUuid);
}

//  信号槽　初始化
void FileViewWidget::initConnections()
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotCustomContextMenuRequested(const QPoint &)));

    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)), SLOT(slotDealWithData(const int &, const QString &)));
    connect(this, SIGNAL(sigDealWithKeyMsg(const QString &)), SLOT(slotDealWithKeyMsg(const QString &)));
    connect(this, SIGNAL(sigCloseFile(const QString &)), SLOT(SlotCloseFile(const QString &)));
}

//  打印
void FileViewWidget::onPrintFile()
{
    PrintManager p;
    p.showPrintDialog(this);
}

//  设置　窗口　自适应　宽＼高　度
void FileViewWidget::onSetWidgetAdapt()
{
    DocummentProxy *_proxy = DocummentProxy::instance(m_strProcUuid);
    if (!_proxy)
        return;

    double nScale = 0.0;
    if (m_nAdapteState == WIDGET_State) {
        int nWidth = this->width();
        nScale = _proxy->adaptWidthAndShow(nWidth);
    } else if (m_nAdapteState == HEIGHT_State) {
        int nHeight = this->height();
        nScale = _proxy->adaptHeightAndShow(nHeight);
    }

    if (nScale != 0.0) {
        notifyMsg(MSG_FILE_SCALE, QString::number(nScale));
    }
}

//  消息 数据 处理
int FileViewWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (m_pKeyMsgList.contains(msgContent)) {
            emit sigDealWithKeyMsg(msgContent);
            return ConstantMsg::g_effective_res;
        }
    }

    if (msgType == MSG_VIEWCHANGE_DOUBLE_SHOW) {
        m_bDoubleShow = msgContent.toInt();
        OnSetViewChange();
    } else if (msgType == MSG_CLOSE_FILE) {
        emit sigCloseFile(msgContent);
    }

    return 0;
}

void FileViewWidget::qSetBindPath(const QString &sPath)
{
    m_strPath = sPath;
    dApp->m_pDataManager->qInsertFileAndUuid(m_strPath, m_strProcUuid);
}
