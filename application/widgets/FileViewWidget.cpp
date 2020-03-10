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

#include <QFileInfo>

#include "FindWidget.h"

#include "business/FileFormatHelper.h"
#include "docview/commonstruct.h"
#include "docview/docummentproxy.h"
#include "main/MainTabWidgetEx.h"
#include "menu/DefaultOperationMenu.h"
#include "menu/TextOperationMenu.h"
#include "utils/PublicFunction.h"
#include "business/PrintManager.h"

#include "gof/bridge/IHelper.h"
#include "main/MainTabWidgetEx.h"
#include "pdfControl/note/NoteViewWidget.h"
#include "widgets/NoteTipWidget.h"

#include "controller/Annotation.h"
#include "controller/FileViewWidgetPrivate.h"

FileViewWidget::FileViewWidget(CustomWidget *parent)
    : CustomWidget(FILE_VIEW_WIDGET, parent),
      m_operatemenu(new TextOperationMenu(this)),
      m_pDefaultMenu(new DefaultOperationMenu(this))
{
    d_fvptr = new FileViewWidgetPrivate(this);

    m_pMsgList = { MSG_HANDLESHAPE,
                   MSG_NOTE_ADD_CONTENT,
                   MSG_VIEWCHANGE_DOUBLE_SHOW, MSG_VIEWCHANGE_ROTATE, MSG_FILE_SCALE, MSG_VIEWCHANGE_FIT,
                   MSG_OPERATION_TEXT_ADD_ANNOTATION,
                   MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, MSG_NOTE_PAGE_SHOW_NOTEWIDGET
                 };
    m_pKeyMsgList = {KeyStr::g_ctrl_l, KeyStr::g_ctrl_i, KeyStr::g_ctrl_c};

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
    Q_D(FileViewWidget);
    d->mouseMoveEvent(event);
}

//  鼠标左键 按下
void FileViewWidget::mousePressEvent(QMouseEvent *event)
{
    Q_D(FileViewWidget);
    d->mousePressEvent(event);
}

//  鼠标松开
void FileViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(FileViewWidget);
    d->mouseReleaseEvent(event);
}

//  文档 显示区域 大小变化
void FileViewWidget::resizeEvent(QResizeEvent *event)
{
    if (!m_bFirstShow) {
        onSetWidgetAdapt();
    }

    if (m_pFindWidget && m_pFindWidget->isVisible()) {
        int nParentWidth = this->width();
        m_pFindWidget->showPosition(nParentWidth);
    }

    CustomWidget::resizeEvent(event);
}

void FileViewWidget::wheelEvent(QWheelEvent *event)
{
    if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
        QJsonObject obj;
        obj.insert("type", "ShortCut");

        if (event->delta() > 0) {
            obj.insert("key", KeyStr::g_ctrl_larger);
        } else {
            obj.insert("key", KeyStr::g_ctrl_smaller);
        }
        QJsonDocument doc = QJsonDocument(obj);
        notifyMsg(E_APP_MSG_TYPE, doc.toJson(QJsonDocument::Compact));
    }
    CustomWidget::wheelEvent(event);
}

//  弹出 自定义 菜单
void FileViewWidget::slotCustomContextMenuRequested(const QPoint &point)
{
    //  处于幻灯片模式下
    int nState = MainTabWidgetEx::Instance()->getCurrentState();
    if (nState == SLIDER_SHOW)
        return;

    //  放大镜状态， 直接返回
    if (nState == Magnifer_State)
        return;

    //  手型状态， 直接返回
    if (nState == Handel_State)
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
        m_operatemenu->setClickPoint(pRightClickPoint);
        m_operatemenu->setPStartPoint(m_pStartPoint);
        m_operatemenu->setPEndPoint(m_pEndSelectPoint);
        m_operatemenu->setClickPage(nPage);
        m_operatemenu->setType(NOTE_ICON);
        dApp->m_pAppInfo->setMousePressLocal(bIsHighLight, tempPoint);
        m_operatemenu->execMenu(tempPoint, bicon, sSelectText, struuid);
    } else {  //  否则弹出 文档操作菜单
        m_pDefaultMenu->setClickpoint(pRightClickPoint);
        m_pDefaultMenu->execMenu(tempPoint, nPage);

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
    if (nRes == Handel_State) {  //  手形
        __SetCursor(Qt::OpenHandCursor);
    } else {
        __SetCursor(Qt::ArrowCursor);
    }
    MainTabWidgetEx::Instance()->setCurrentState(nRes);
}

//  添加高亮颜色  快捷键
void FileViewWidget::onFileAddAnnotation()
{
    Q_D(FileViewWidget);
    //  处于幻灯片模式下
    int nState = MainTabWidgetEx::Instance()->getCurrentState();
    if (nState == SLIDER_SHOW)
        return;

    //  放大镜状态， 直接返回
    if (nState == Magnifer_State)
        return;

    //  手型状态， 直接返回
    if (nState == Handel_State)
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

        //dApp->m_pHelper->qDealWithData(MSG_NOTE_ADD_HIGHLIGHT_COLOR, sContent);
        d->AddHighLight(sContent);
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

        Q_D(FileViewWidget);
        d->slotDealWithMenu(MSG_NOTE_ADD_HIGHLIGHT_NOTE, sContent);
    }
}

void FileViewWidget::onFileAddNote()
{
    if (!m_pProxy)
        return;

    //  处于幻灯片模式下
    int nState = MainTabWidgetEx::Instance()->getCurrentState();
    if (nState == SLIDER_SHOW)
        return;

    //  放大镜状态， 直接返回
    if (nState == Magnifer_State)
        return;

    //  手型状态， 直接返回
    if (nState == Handel_State)
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
    QJsonObject obj;
    obj.insert("content", msgContent);
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_OPERATION_TEXT_ADD_ANNOTATION, doc.toJson(QJsonDocument::Compact));
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

        m_nAdapteState = fdm.qGetData(Fit);

        int curPage = fdm.qGetData(CurPage);
        int iscale = m_nScale = fdm.qGetData(Scale);         // 缩放
        int doubPage = m_nDoubleShow = fdm.qGetData(DoubleShow);   // 是否是双页
        int rotate = m_rotateType = fdm.qGetData(Rotate);         // 文档旋转角度(0,1,2,3,4)

        iscale = (iscale > 500 ? 500 : iscale) <= 0 ? 100 : iscale;
        double scale = iscale / 100.0;
        RotateType_EM rotatetype = static_cast<RotateType_EM>(rotate);
        ViewMode_EM viewmode = static_cast<ViewMode_EM>(doubPage);

        bool rl = m_pProxy->openFile(nCurDocType, sPath, curPage, rotatetype, scale, viewmode);
        if (rl) {

            Q_D(FileViewWidget);
            d->setProxy(m_pProxy);

            MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
            if (pMtwe) {
                pMtwe->SetFileData(sPath, fdm);
                pMtwe->InsertPathProxy(sPath, m_pProxy);       //  存储 filePath 对应的 proxy
            }

            return rl;
        }
    }
    return false;
}

void FileViewWidget::OnShortCutKey_Ctrl_l()
{
    onFileAddAnnotation();
}

void FileViewWidget::OnShortCutKey_Ctrl_i()
{
    if (m_pProxy) {
        QString selectText;
        if (m_pProxy->getSelectTextString(selectText))
            onFileAddNote();
        else {
            notifyMsg(CENTRAL_SHOW_TIP, tr("Please select the text"));
        }
    }
}

void FileViewWidget::OnShortCutKey_Ctrl_c()
{
    if (m_pProxy) {
        QString sSelectText = "";
        if (m_pProxy->getSelectTextString(sSelectText)) { //  选择　当前选中下面是否有文字
            Utils::copyText(sSelectText);
        }
    }
}

//  比例调整了, 取消自适应 宽高状态
void FileViewWidget::OnSetViewScale(const QString &msgConent)
{
    m_nScale = msgConent.toInt();

    setScaleRotateViewModeAndShow();

    m_nAdapteState = Default_State;
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

    QJsonObject obj;
    obj.insert("content", QString::number(m_rotateType));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_VIEWCHANGE_ROTATE_VALUE, doc.toJson(QJsonDocument::Compact));

    setScaleRotateViewModeAndShow();

    //  旋转之后, 若是 双页显示
    onSetWidgetAdapt();
}

void FileViewWidget::OnSetViewHit(const QString &msgContent)
{
    m_nAdapteState = msgContent.toInt();

    onSetWidgetAdapt();
}

//  文档书签状态改变
void FileViewWidget::slotBookMarkStateChange(int nPage, bool bState)
{
    QJsonObject obj;
    obj.insert("content", QString::number(nPage));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET + Constant::sQStringSep + BOOKMARK_WIDGET);

    QJsonDocument doc(obj);

    if (!bState) {
        notifyMsg(MSG_OPERATION_DELETE_BOOKMARK, doc.toJson(QJsonDocument::Compact));
    } else {
        notifyMsg(MSG_OPERATION_ADD_BOOKMARK, doc.toJson(QJsonDocument::Compact));
    }
}

//  文档页变化了
void FileViewWidget::slotDocFilePageChanged(int page)
{
    QJsonObject obj;
    obj.insert("content", QString::number(page));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_FILE_PAGE_CHANGE, doc.toJson(QJsonDocument::Compact));
}

//  打开文档结果
void FileViewWidget::SlotDocFileOpenResult(bool openresult)
{
    //  通知 其他窗口， 打开文件成功了！！！
    if (openresult) {
        dApp->m_pDBService->qSelectData(m_strPath, DB_BOOKMARK);

        emit sigFileOpenOK(m_strPath);

        onSetWidgetAdapt();

        m_bFirstShow = false;
    } else {
        notifyMsg(MSG_OPERATION_OPEN_FILE_FAIL, tr("Please check if the file is damaged"));
    }
}

void FileViewWidget::SlotFindOperation(const int &iType, const QString &strFind)
{
    emit sigFindOperation(iType);

    if (iType == E_FIND_CONTENT || iType == E_FIND_EXIT) {
        notifyMsg(iType, m_strPath);
    }

    if (m_pProxy) {
        if (iType == E_FIND_NEXT) {
            m_pProxy->findNext();
        } else if (iType == E_FIND_PREV) {
            m_pProxy->findPrev();
        } else if (iType == E_FIND_EXIT) {
            m_pProxy->clearsearch();
        } else {
            m_pProxy->search(strFind);
        }
    }
}

//  信号槽　初始化
void FileViewWidget::initConnections()
{
    Q_D(FileViewWidget);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotCustomContextMenuRequested(const QPoint &)));
    connect(m_operatemenu, SIGNAL(sigActionTrigger(const int &, const QString &)), d, SLOT(slotDealWithMenu(const int &, const QString &)));
}

//  设置　窗口　自适应　宽＼高　度
void FileViewWidget::onSetWidgetAdapt()
{
    if (m_nAdapteState != Default_State) {
        if (!m_pProxy)
            return;

        double dScale = 0.0;
        if (m_nAdapteState == ADAPTE_WIDGET_State) {
            int nWidth = this->width();
            dScale = m_pProxy->adaptWidthAndShow(nWidth);
        } else if (m_nAdapteState == ADAPTE_HEIGHT_State) {
            int nHeight = this->height();
            dScale = m_pProxy->adaptHeightAndShow(nHeight);
        }

        if (dScale != 0.0) {
            notifyMsg(MSG_FILE_FIT_SCALE, QString::number(dScale));
        }
    }
}

//  消息 数据 处理
int FileViewWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pFindWidget) {
        int nRes = m_pFindWidget->dealWithData(msgType, msgContent);
        if (nRes == MSG_OK) {
            return nRes;
        }
    }

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
    } else if (msgType == MSG_NOTE_ADD_CONTENT) {
        onFileAddNote(msgContent);
    } else if (msgType == MSG_OPERATION_TEXT_ADD_ANNOTATION) {             //  添加注释
        onOpenNoteWidget(msgContent);
    } else if (msgType == MSG_OPERATION_TEXT_SHOW_NOTEWIDGET) {
        onShowNoteWidget(msgContent);
    } else if (msgType == MSG_NOTE_PAGE_SHOW_NOTEWIDGET) {          //  显示注释窗口
        __ShowPageNoteWidget(msgContent);
    }

    bool rl = m_pMsgList.contains(msgType);
    if (rl) {
        return MSG_OK;
    }

    return MSG_NO_OK;
}

int FileViewWidget::qDealWithShortKey(const QString &sKey)
{
    if (sKey == KeyStr::g_ctrl_l) {
        OnShortCutKey_Ctrl_l();
    } else if (sKey == KeyStr::g_ctrl_i) {
        OnShortCutKey_Ctrl_i();
    } else if (sKey == KeyStr::g_ctrl_c) {
        OnShortCutKey_Ctrl_c();
    }

    if (m_pKeyMsgList.contains(sKey)) {
        return MSG_OK;
    }
    return MSG_NO_OK;
}

void FileViewWidget::setFileChange(bool bchanged)
{
    Q_D(FileViewWidget);
    d->m_filechanged = bchanged;
}

bool FileViewWidget::getFileChange()
{
    Q_D(FileViewWidget);
    return d->m_filechanged ;
}

void FileViewWidget::ShowFindWidget()
{
    if (m_pFindWidget == nullptr) {
        m_pFindWidget = new FindWidget(this);
        connect(m_pFindWidget, SIGNAL(sigFindOperation(const int &, const QString &)), SLOT(SlotFindOperation(const int &, const QString &)));
    }

    int nParentWidth = this->width();
    m_pFindWidget->showPosition(nParentWidth);
    m_pFindWidget->setSearchEditFocus();
}

void FileViewWidget::onOpenNoteWidget(const QString &msgContent)
{
    Q_D(FileViewWidget);

    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 3) {

        QString sPage = sList.at(0);
//        QString sX = sList.at(1);
//        QString sY = sList.at(2);

        if (m_pNoteViewWidget == nullptr) {
            m_pNoteViewWidget = new NoteViewWidget(this);
            connect(m_pNoteViewWidget, SIGNAL(sigNoteViewMsg(const int &, const QString &)), d, SLOT(SlotNoteViewMsg(const int &, const QString &)));
        }
        m_pNoteViewWidget->setEditText("");
        m_pNoteViewWidget->setNoteUuid("");
        m_pNoteViewWidget->setNotePage(sPage);
        m_pNoteViewWidget->setWidgetType(NOTE_HIGHLIGHT);

        QPoint point;
        bool t_bHigh = false;
        dApp->m_pAppInfo->setSmallNoteWidgetSize(m_pNoteViewWidget->size());
        dApp->m_pAppInfo->mousePressLocal(t_bHigh, point);
        m_pNoteViewWidget->showWidget(point);
    }
}

//  显示 当前 注释
void FileViewWidget::onShowNoteWidget(const QString &contant)
{
    Q_D(FileViewWidget);
    QStringList t_strList = contant.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (t_strList.count() == 2) {
        QString t_strUUid = t_strList.at(0);
        QString t_page = t_strList.at(1);

        QString sContant = "";

        auto pHelper = MainTabWidgetEx::Instance()->getCurFileAndProxy();
        if (pHelper) {
            pHelper->getAnnotationText(t_strUUid, sContant, t_page.toInt());
        }

        if (m_pNoteViewWidget == nullptr) {
            m_pNoteViewWidget = new NoteViewWidget(this);
            connect(m_pNoteViewWidget, SIGNAL(sigNoteViewMsg(const int &, const QString &)), d, SLOT(SlotNoteViewMsg(const int &, const QString &)));
        }
        m_pNoteViewWidget->setNoteUuid(t_strUUid);
        m_pNoteViewWidget->setNotePage(t_page);
        m_pNoteViewWidget->setEditText(sContant);
        m_pNoteViewWidget->setWidgetType(NOTE_HIGHLIGHT);
        dApp->m_pAppInfo->setSmallNoteWidgetSize(m_pNoteViewWidget->size());

        bool t_bHigh = false;  // 点击位置是否是高亮
        QPoint point;          // = this->mapToGlobal(rrect.bottomRight());// 鼠标点击位置

        dApp->m_pAppInfo->mousePressLocal(t_bHigh, point);
        m_pNoteViewWidget->showWidget(point);
    }
}

void FileViewWidget::__ShowPageNoteWidget(const QString &msgContent)
{
    Q_D(FileViewWidget);
    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 4) {
        QString sUuid = sList.at(0);
        QString sPage = sList.at(1);
        QString sX = sList.at(2);
        QString sY = sList.at(3);

        QString sContant = "";

        auto pHelper = MainTabWidgetEx::Instance()->getCurFileAndProxy();
        if (pHelper) {
            pHelper->getAnnotationText(sUuid, sContant, sPage.toInt());
        }
        if (m_pNoteViewWidget == nullptr) {
            m_pNoteViewWidget = new NoteViewWidget(this);
            connect(m_pNoteViewWidget, SIGNAL(sigNoteViewMsg(const int &, const QString &)), d, SLOT(SlotNoteViewMsg(const int &, const QString &)));
        }
        m_pNoteViewWidget->setEditText(sContant);
        m_pNoteViewWidget->setNoteUuid(sUuid);
        m_pNoteViewWidget->setNotePage(sPage);
        m_pNoteViewWidget->setWidgetType(NOTE_ICON);
        m_pNoteViewWidget->showWidget(QPoint(sX.toInt(), sY.toInt()));
    }
}

void FileViewWidget::__ShowNoteTipWidget(const QString &sText)
{
    if (m_pTipWidget == nullptr) {
        m_pTipWidget = new NoteTipWidget(this);
    }
    m_pTipWidget->setTipContent(sText);
    QPoint showRealPos(QCursor::pos().x(), QCursor::pos().y() + 10);
    m_pTipWidget->move(showRealPos);
    m_pTipWidget->show();
}


void FileViewWidget::__CloseFileNoteWidget()
{
    if (m_pTipWidget && m_pTipWidget->isVisible()) {
        m_pTipWidget->close();
    }
}
