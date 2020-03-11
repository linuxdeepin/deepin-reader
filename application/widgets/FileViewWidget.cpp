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

#include "widgets/controller/ProxyViewDisplay.h"

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

#include "controller/FileViewWidgetPrivate.h"

FileViewWidget::FileViewWidget(CustomWidget *parent)
    : CustomWidget(FILE_VIEW_WIDGET, parent),
      m_operatemenu(new TextOperationMenu(this)),
      m_pDefaultMenu(new DefaultOperationMenu(this))
{
    d_fvptr = new FileViewWidgetPrivate(this);

    m_pMsgList = { MSG_HANDLESHAPE,
                   MSG_VIEWCHANGE_DOUBLE_SHOW, MSG_VIEWCHANGE_ROTATE, MSG_FILE_SCALE, MSG_VIEWCHANGE_FIT,
                   MSG_OPERATION_TEXT_ADD_ANNOTATION,
                   MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, MSG_NOTE_PAGE_SHOW_NOTEWIDGET
                 };
    m_pKeyMsgList = {KeyStr::g_ctrl_l, KeyStr::g_ctrl_i, KeyStr::g_ctrl_c};

    setMouseTracking(true);  //  接受 鼠标滑动事件

    m_pDocViewProxy = new ProxyViewDisplay(this);

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
    Q_D(FileViewWidget);
    if (!d->m_bFirstShow) {
        m_pDocViewProxy->setWidth(this->width());
        m_pDocViewProxy->setHeight(this->height());
        m_pDocViewProxy->onSetWidgetAdapt();
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

    QString sSelectText = "";
    m_pProxy->getSelectTextString(sSelectText);  //  选择　当前选中下面是否有文字

    QPoint tempPoint = this->mapToGlobal(point);
    QPoint pRightClickPoint = m_pProxy->global2RelativePoint(tempPoint);

    //  右键鼠标点 是否有高亮区域
    QString sAnnotationText = "", struuid = "";
    bool bIsHighLight = m_pProxy->annotationClicked(pRightClickPoint, sAnnotationText, struuid);
    bool bicon = m_pProxy->iconAnnotationClicked(pRightClickPoint, sAnnotationText, struuid);
    int nPage = m_pProxy->pointInWhichPage(pRightClickPoint);

    if (sSelectText != "") {
        m_operatemenu->setClickPoint(pRightClickPoint);
        m_operatemenu->setPStartPoint(d->m_pStartPoint);
        m_operatemenu->setPEndPoint(d->m_pEndSelectPoint);
        m_operatemenu->setClickPage(nPage);
        dApp->m_pAppInfo->setMousePressLocal(bIsHighLight, tempPoint);
        bool bremoveenable = false;
        if (bicon) {
            m_operatemenu->setType(NOTE_ICON);
        } else {
            m_operatemenu->setType(NOTE_HIGHLIGHT);
        }
        if (bicon || bIsHighLight)
            bremoveenable = true;
        qDebug() << "*********";
        m_operatemenu->setRemoveEnabled(bremoveenable);
        m_operatemenu->execMenu(tempPoint, true, sSelectText, struuid);
    } else if (sSelectText == "" && (bIsHighLight || bicon)) { //  选中区域 有文字, 弹出 文字操作菜单
        //  需要　区别　当前选中的区域，　弹出　不一样的　菜单选项
        m_operatemenu->setClickPoint(pRightClickPoint);
        m_operatemenu->setPStartPoint(d->m_pStartPoint);
        m_operatemenu->setPEndPoint(d->m_pEndSelectPoint);
        m_operatemenu->setClickPage(nPage);
        dApp->m_pAppInfo->setMousePressLocal(bIsHighLight, tempPoint);
        if (bicon) {
            m_operatemenu->setType(NOTE_ICON);
        } else if (bIsHighLight) {
            m_operatemenu->setType(NOTE_HIGHLIGHT);
        }
        sSelectText = sAnnotationText;
        m_operatemenu->setRemoveEnabled(true);
        m_operatemenu->execMenu(tempPoint, false, sSelectText, struuid);
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
//void FileViewWidget::onFileAddAnnotation()
//{
//    Q_D(FileViewWidget);
//    //  处于幻灯片模式下
//    int nState = MainTabWidgetEx::Instance()->getCurrentState();
//    if (nState == SLIDER_SHOW)
//        return;

//    //  放大镜状态， 直接返回
//    if (nState == Magnifer_State)
//        return;

//    //  手型状态， 直接返回
//    if (nState == Handel_State)
//        return;

//    int nSx = d->m_pStartPoint.x();
//    int nSy = d->m_pStartPoint.y();

//    int nEx = d->m_pEndSelectPoint.x();
//    int nEy = d->m_pEndSelectPoint.y();

//    if (nSx == nEx && nSy == nEy) {
//        notifyMsg(CENTRAL_SHOW_TIP, tr("Please select the text"));
//        return;
//    }

//    if (!m_pProxy)
//        return;

//    QString selectText = "";
//    m_pProxy->getSelectTextString(selectText);
//    if (selectText != "") {
//        QString sContent = QString::number(nSx) + Constant::sQStringSep +
//                           QString::number(nSy) + Constant::sQStringSep +
//                           QString::number(nEx) + Constant::sQStringSep +
//                           QString::number(nEy);

//        d->AddHighLight(sContent);
//    } else {
//        notifyMsg(CENTRAL_SHOW_TIP, tr("Please select the text"));
//    }
//}

//void FileViewWidget::onFileAddNote()
//{
//    if (!m_pProxy)
//        return;

//    Q_D(FileViewWidget);

//    //  处于幻灯片模式下
//    int nState = MainTabWidgetEx::Instance()->getCurrentState();
//    if (nState == SLIDER_SHOW)
//        return;

//    //  放大镜状态， 直接返回
//    if (nState == Magnifer_State)
//        return;

//    //  手型状态， 直接返回
//    if (nState == Handel_State)
//        return;

//    int nSx = d->m_pStartPoint.x();
//    int nSy = d->m_pStartPoint.y();

//    int nEx = d->m_pEndSelectPoint.x();
//    int nEy = d->m_pEndSelectPoint.y();

//    if ((nSx == nEx && nSy == nEy)) {
//        notifyMsg(CENTRAL_SHOW_TIP, tr("Please select the text"));
//        return;
//    }

//    int nPage = m_pProxy->pointInWhichPage(d->m_pEndSelectPoint);
//    QString msgContent = QString("%1").arg(nPage) + Constant::sQStringSep +
//                         QString("%1").arg(d->m_pEndSelectPoint.x()) + Constant::sQStringSep +
//                         QString("%1").arg(d->m_pEndSelectPoint.y());

//    d->slotDealWithMenu(MSG_OPERATION_TEXT_ADD_ANNOTATION, msgContent);
//}

//  设置鼠标状态
void FileViewWidget::__SetCursor(const QCursor &cs)
{
    const QCursor ss = cursor();    //  当前鼠标状态
    if (ss != cs) {
        setCursor(cs);
    }
}

void FileViewWidget::OpenFilePath(const QString &sPath)
{
    Q_D(FileViewWidget);
    d->m_strPath = sPath;
    //  实际文档类  唯一实例化设置 父窗口
    m_pProxy = new DocummentProxy(this);
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

        int nAdapteState = fdm.qGetData(Fit);
        m_pDocViewProxy->setAdapteState(nAdapteState);

        int curPage = fdm.qGetData(CurPage);
        int iscale  = fdm.qGetData(Scale);         // 缩放
        m_pDocViewProxy->setScale(iscale);

        int doubPage = fdm.qGetData(DoubleShow);   // 是否是双页
        m_pDocViewProxy->setDoubleShow(doubPage);

        int rotate = fdm.qGetData(Rotate);         // 文档旋转角度(0,1,2,3,4)
        m_pDocViewProxy->setRotateType(rotate);

        iscale = (iscale > 500 ? 500 : iscale) <= 0 ? 100 : iscale;
        double scale = iscale / 100.0;
        RotateType_EM rotatetype = static_cast<RotateType_EM>(rotate);
        ViewMode_EM viewmode = static_cast<ViewMode_EM>(doubPage);

        bool rl = m_pProxy->openFile(nCurDocType, sPath, curPage, rotatetype, scale, viewmode);
        if (rl) {
            m_pProxy->setViewFocus();
            m_pDocViewProxy->setProxy(m_pProxy);
            d->setProxy(m_pProxy);

            MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
            if (pMtwe) {
                pMtwe->SetFileData(sPath, fdm);
                pMtwe->InsertPathProxy(sPath, m_pProxy);       //  存储 filePath 对应的 proxy
            }
        }
    }
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
        Q_D(FileViewWidget);
        dApp->m_pDBService->qSelectData(d->m_strPath, DB_BOOKMARK);

        emit sigFileOpenOK(d->m_strPath);

        m_pDocViewProxy->onSetWidgetAdapt();

        d->m_bFirstShow = false;
    } else {
        notifyMsg(MSG_OPERATION_OPEN_FILE_FAIL, tr("Please check if the file is damaged"));
    }
}

void FileViewWidget::SlotFindOperation(const int &iType, const QString &strFind)
{
    Q_D(FileViewWidget);
    emit sigFindOperation(iType);

    if (iType == E_FIND_CONTENT || iType == E_FIND_EXIT) {
        notifyMsg(iType, d->m_strPath);
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
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotCustomContextMenuRequested(const QPoint &)));

    Q_D(FileViewWidget);
    connect(m_operatemenu, SIGNAL(sigActionTrigger(const int &, const QString &)), d, SLOT(slotDealWithMenu(const int &, const QString &)));
    connect(this, SIGNAL(sigDeleteAnntation(const int &, const QString &)), d, SLOT(SlotDeleteAnntation(const int &, const QString &)));
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
        m_pDocViewProxy->OnSetViewChange(msgContent);
    } else if (msgType == MSG_VIEWCHANGE_ROTATE) {
        m_pDocViewProxy->OnSetViewRotate(msgContent);
    } else if (msgType == MSG_FILE_SCALE) {
        m_pDocViewProxy->OnSetViewScale(msgContent);
    } else if (msgType == MSG_VIEWCHANGE_FIT) {
        m_pDocViewProxy->OnSetViewHit(msgContent);
    } else if (msgType == MSG_HANDLESHAPE) {
        onSetHandShape(msgContent);
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
    if (m_pKeyMsgList.contains(sKey)) {
        Q_D(FileViewWidget);
        d->OnShortCutKey(sKey);
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

        m_pProxy->getAnnotationText(t_strUUid, sContant, t_page.toInt());

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

        m_pProxy->getAnnotationText(sUuid, sContant, sPage.toInt());

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
