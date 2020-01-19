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

#include "application.h"

#include "docview/docummentproxy.h"
#include "controller/DataManager.h"
#include "controller/AppSetting.h"
#include "mainShow/DefaultOperationMenu.h"
#include "utils/PublicFunction.h"
#include "dialog/PrintManager.h"

FileViewWidget::FileViewWidget(CustomWidget *parent)
    : CustomWidget("FileViewWidget", parent)
    , m_operatemenu(nullptr)
{
    m_pMsgList = { MSG_MAGNIFYING, MSG_HANDLESHAPE, MSG_SELF_ADAPTE_HEIGHT, MSG_SELF_ADAPTE_WIDTH,
                   MSG_FILE_ROTATE, MSG_OPERATION_TEXT_ADD_HIGHLIGHTED,
                   MSG_OPERATION_TEXT_UPDATE_HIGHLIGHTED, MSG_OPERATION_TEXT_REMOVE_HIGHLIGHTED,
                   MSG_NOTE_ADDCONTANT
                 };

    m_pKeyMsgList = {KeyStr::g_ctrl_p, KeyStr::g_ctrl_l, KeyStr::g_ctrl_i};

    setMouseTracking(true);  //  接受 鼠标滑动事件

    initWidget();
    initConnections();

    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

FileViewWidget::~FileViewWidget()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

void FileViewWidget::initWidget()
{
    //  实际文档类  唯一实例化设置 父窗口
    auto m_pDocummentProxy = DocummentProxy::instance(this);
    if (m_pDocummentProxy) {
        connect(m_pDocummentProxy, SIGNAL(signal_bookMarkStateChange(int, bool)), this,
                SLOT(slotBookMarkStateChange(int, bool)));
        connect(m_pDocummentProxy, SIGNAL(signal_pageChange(int)), this,
                SLOT(slotDocFilePageChanged(int)));

        connect(m_pDocummentProxy, SIGNAL(signal_openResult(bool)), SLOT(SlotDocFileOpenResult(bool)));

    }

    m_pDocummentFileHelper = DocummentFileHelper::instance();
//        m_pDocummentFileHelper->setDocProxy(m_pDocummentProxy);     //  唯一设置 文档类入口
}

//  鼠标移动
void FileViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
        return;

    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
        emit(sigShowPlayCtrl(true));
        return;
    }

    QPoint globalPos = event->globalPos();
    QPoint docGlobalPos = _proxy->global2RelativePoint(globalPos);
    //begin>>kyz 2019-12-19 注释图标移动测试
//    if (m_bmousepressed && event->buttons()&Qt::LeftButton && !struidtmp.isEmpty()) {

//        DocummentProxy::instance()->moveIconAnnotation(struidtmp, docGlobalPos);
//    }
    //end<<
    if (m_nCurrentHandelState == Handel_State) {  //   手型状态下， 按住鼠标左键 位置进行移动
        if (m_bSelectOrMove) {
            QPoint mvPoint = m_pHandleMoveStartPoint - globalPos;
            int mvX = mvPoint.x();
            int mvY = mvPoint.y();

            _proxy->pageMove(mvX, mvY);

            m_pHandleMoveStartPoint = globalPos;
        }
    } else if (m_nCurrentHandelState == Magnifier_State) {  //  当前是放大镜状态
        _proxy->showMagnifier(docGlobalPos);
    } else {
        if (m_bSelectOrMove) {  //  鼠标已经按下，　则选中所经过的文字
            m_pEndSelectPoint = docGlobalPos;
            _proxy->mouseSelectText(m_pStartPoint, m_pEndSelectPoint);
        } else {
            //  首先判断文档划过属性
            auto pLink = _proxy->mouseBeOverLink(docGlobalPos);
            if (pLink) {
                setCursor(QCursor(Qt::PointingHandCursor));
            } else {
                if (_proxy->mouseBeOverText(docGlobalPos)) {
                    m_bIsHandleSelect = true;
                    setCursor(QCursor(Qt::IBeamCursor));

                    onShowNoteTipWidget(docGlobalPos);
                } else {
                    if (m_pNoteTipWidget && m_pNoteTipWidget->isVisible()) {
                        m_pNoteTipWidget->hide();
                    }

                    m_bIsHandleSelect = false;
                    setCursor(QCursor(Qt::ArrowCursor));
                }
            }
        }
    }
}

//  鼠标左键 按下
void FileViewWidget::mousePressEvent(QMouseEvent *event)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
        return;

    m_bmousepressed = true;
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
        return;
    }
    //  放大镜状态， 直接返回
    if (m_nCurrentHandelState == Magnifier_State)
        return;

    //begin>>kyz 2019-12-18 测试
//    QPoint pt = m_pDocummentFileHelper->global2RelativePoint(event->globalPos());
//    if (event->button() == Qt::RightButton)
//        DocummentProxy::instance()->addIconAnnotation(pt);
//    else {
//        QString strtext, struid;
//        DocummentProxy::instance()->iconAnnotationClicked(pt, strtext, struidtmp);
//    }
    //<<end

    Qt::MouseButton nBtn = event->button();
    if (nBtn == Qt::LeftButton) {
        QPoint globalPos = event->globalPos();

        //  当前状态是 手, 先 拖动, 之后 在是否是链接之类
        if (m_nCurrentHandelState == Handel_State) {
            m_pHandleMoveStartPoint = globalPos;  //  变成手，　需要的是　相对坐标
            m_bSelectOrMove = true;
        } else {
            QPoint docGlobalPos = _proxy->global2RelativePoint(globalPos);

            //  点击的时候　先判断　点击处　　是否有链接之类
            auto pLink = _proxy->mouseBeOverLink(docGlobalPos);
            if (pLink) {
                __ClickPageLink(pLink);
            } else {
                if (m_nCurrentHandelState == Default_State) {
                    _proxy->mouseSelectTextClear();  //  清除之前选中的文字高亮

                    if (m_pNoteTipWidget && m_pNoteTipWidget->isVisible()) {
                        m_pNoteTipWidget->hide();
                    }

                    if (m_bIsHandleSelect) {
                        m_bSelectOrMove = true;
                        m_pStartPoint = docGlobalPos;
                        m_pEndSelectPoint = m_pStartPoint;
                    }
                }
            }
        }
    }
}

//  鼠标松开
void FileViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
        return;

    m_bmousepressed = false;
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
        if (event->button() == Qt::RightButton)
            notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_esc);
        return;
    }

    //  放大镜状态， 右键则取消放大镜 并且 直接返回
    Qt::MouseButton nBtn = event->button();
    if (nBtn == Qt::RightButton && m_nCurrentHandelState == Magnifier_State) {
        notifyMsg(MSG_MAGNIFYING_CANCEL);
        return;
    }

    if (m_bSelectOrMove) {
        //判断鼠标左键松开的位置有没有高亮
//        qDebug() << __FUNCTION__ << _proxy->global2RelativePoint(event->globalPos()) << m_pStartPoint;
        QPoint globalPos = event->globalPos();
        QPoint docGlobalPos = _proxy->global2RelativePoint(globalPos);
        DataManager::instance()->setMousePressLocal(false, globalPos);
        //添加其实结束point是否为同一个，不是同一个说明不是点击可能是选择文字
        if (nBtn == Qt::LeftButton && docGlobalPos == m_pStartPoint) {
            // 判断鼠标点击的地方是否有高亮
            QString selectText, t_strUUid;

            bool bIsHighLightReleasePoint = _proxy->annotationClicked(docGlobalPos, selectText, t_strUUid);
            DataManager::instance()->setMousePressLocal(bIsHighLightReleasePoint, globalPos);
            if (bIsHighLightReleasePoint) {
                if (m_pNoteTipWidget && m_pNoteTipWidget->isVisible()) {
                    m_pNoteTipWidget->hide();
                }

                int nPage = _proxy->pointInWhichPage(docGlobalPos);
                QString t_strContant =
                    t_strUUid.trimmed() + QString("%1%") + QString::number(nPage);
                notifyMsg(MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, t_strContant);
            }
        }
    }

    m_bSelectOrMove = false;
    CustomWidget::mouseReleaseEvent(event);
}

void FileViewWidget::leaveEvent(QEvent *event)
{
    if (m_pNoteTipWidget && m_pNoteTipWidget->isVisible()) {
        m_pNoteTipWidget->hide();
    }
    CustomWidget::leaveEvent(event);
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
        QString sFilePath = DataManager::instance()->strOnlyFilePath();
        if (sFilePath != "") {
            if (event->delta() > 0) {
                notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_larger);
            } else {
                notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_smaller);
            }
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
    case MSG_SELF_ADAPTE_HEIGHT:    //  自适应 高度
        if (msgContent == "1") {
            m_nAdapteState = HEIGHT_State;
            onSetWidgetAdapt();
        } else {
            m_nAdapteState = Default_State;
        }
        break;
    case MSG_SELF_ADAPTE_WIDTH:     //  自适应宽度
        if (msgContent == "1") {
            m_nAdapteState = WIDGET_State;
            onSetWidgetAdapt();
        } else {
            m_nAdapteState = Default_State;
        }
        break;
    case MSG_FILE_ROTATE:           //  文档旋转了
        onSetWidgetAdapt();
        break;
    case MSG_OPERATION_TEXT_ADD_HIGHLIGHTED:    //  高亮显示
        onFileAddAnnotation(msgContent);
        break;
    case MSG_OPERATION_TEXT_UPDATE_HIGHLIGHTED: //  更新高亮颜色
        onFileUpdateAnnotation(msgContent);
        break;
    case MSG_OPERATION_TEXT_REMOVE_HIGHLIGHTED: //  移除高亮颜色
        onFileRemoveAnnotation(msgContent);
        break;
    case MSG_NOTE_ADDCONTANT:                   //  添加注释
        onFileAddNote(msgContent);
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
            notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("Please select the text"));
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
    if (DataManager::instance()->CurShowState() == FILE_SLIDE)
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

    int nPage = _proxy->pointInWhichPage(pRightClickPoint);
    if (sSelectText != "" || bIsHighLight) {  //  选中区域 有文字, 弹出 文字操作菜单
        //  需要　区别　当前选中的区域，　弹出　不一样的　菜单选项
        if (nullptr == m_operatemenu) {
            m_operatemenu = new TextOperationMenu(this);
//            m_operatemenu->setFixedWidth(304);
        }
        m_operatemenu->setClickPoint(pRightClickPoint);
        m_operatemenu->setClickPage(nPage);

        DataManager::instance()->setMousePressLocal(bIsHighLight, tempPoint);

        m_operatemenu->execMenu(tempPoint, bIsHighLight, sSelectText, struuid);
    } else {  //  否则弹出 文档操作菜单
        auto menu = new DefaultOperationMenu(this);
//        menu->setFixedWidth(182);
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
        this->setCursor(Qt::BlankCursor);
    } else {
        if (m_nCurrentHandelState == Magnifier_State) {  //  是 放大镜模式 才取消

            m_nCurrentHandelState = Default_State;
            this->setCursor(Qt::ArrowCursor);

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
        this->setCursor(Qt::OpenHandCursor);
    } else {
        m_nCurrentHandelState = Default_State;
        this->setCursor(Qt::ArrowCursor);
    }
}

//  添加高亮颜色  快捷键
void FileViewWidget::onFileAddAnnotation()
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
        return;

    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE)
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
        notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("Please select the text"));
        return;
    }
    QString selectText = "", t_strUUid = "";
    _proxy->getSelectTextString(selectText);
    if (selectText != "") {
        QColor color = DataManager::instance()->selectColor();

        m_pDocummentFileHelper->addAnnotation(m_pStartPoint, m_pEndSelectPoint, color);
    } else {
        notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("Please select the text"));
    }
}

//  添加高亮颜色
void FileViewWidget::onFileAddAnnotation(const QString &msgContent)
{
    QStringList contentList = msgContent.split(",", QString::SkipEmptyParts);
    if (contentList.size() == 3) {
        QString sIndex = contentList.at(0);
        QString sX = contentList.at(1);
        QString sY = contentList.at(2);

        int iIndex = sIndex.toInt();
        QColor color = DataManager::instance()->getLightColorList().at(iIndex);
        DataManager::instance()->setSelectColor(color);

        QPoint tempPoint(sX.toInt(), sY.toInt());
        qDebug() << "FileViewWidget::slotFileAddAnnotation" << m_pStartPoint << m_pEndSelectPoint;
        m_pDocummentFileHelper->addAnnotation(m_pStartPoint, m_pEndSelectPoint, color);
    }
}

//  更新高亮颜色
void FileViewWidget::onFileUpdateAnnotation(const QString &msgContent)
{
    QStringList contentList = msgContent.split(",", QString::SkipEmptyParts);
    if (contentList.size() == 3) {
        QString sIndex = contentList.at(0);
        QString sUuid = contentList.at(1);
        QString sPage = contentList.at(2);

        int iIndex = sIndex.toInt();
        QColor color = DataManager::instance()->getLightColorList().at(iIndex);

        m_pDocummentFileHelper->changeAnnotationColor(sPage.toInt(), sUuid, color);
    }
}

//  移除高亮, 有注释 则删除注释
void FileViewWidget::onFileRemoveAnnotation(const QString &msgContent)
{
    QStringList contentList = msgContent.split(",", QString::SkipEmptyParts);
    if (contentList.size() == 2) {
        QString sX = contentList.at(0);
        QString sY = contentList.at(1);

        QPoint tempPoint(sX.toInt(), sY.toInt());
        QString sUuid = m_pDocummentFileHelper->removeAnnotation(tempPoint);
        if (sUuid != "") {
            sendMsg(MSG_NOTE_DLTNOTEITEM, sUuid);  //  notesWidget 处理该消息
        }
    }
}

//  添加注释
void FileViewWidget::onFileAddNote(const QString &msgContent)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
        return;

    QString sUuid = "", sNote = "", sPage = "";
    int ipage = 0;
    QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (contentList.size() == 3) {
        sNote = contentList.at(0);
        sUuid = contentList.at(1);
        sPage = contentList.at(2);
        ipage = sPage.toInt();
    } else if (contentList.size() == 4) {
        sNote = contentList.at(0);
        sPage = contentList.at(1);
        ipage = sPage.toInt();
        QString sX = contentList.at(2);
        QString sY = contentList.at(3);

        QColor color = DataManager::instance()->selectColor();
        QPoint tempPoint(sX.toInt(), sY.toInt());
        sUuid = m_pDocummentFileHelper->addAnnotation(m_pStartPoint, m_pEndSelectPoint,
                                                      color);  //  高亮 产生的 uuid

        //此判断是针对跨页选择多行并且是page页码从高到低
        if (m_pStartPoint.y() > m_pEndSelectPoint.y()) {
            ipage = _proxy->pointInWhichPage(m_pStartPoint);
        }
    }

    if (sUuid == "" || sNote == "" || sPage == "" || sPage == "-1") {
        return;
    }
    QString t_str = sUuid.trimmed() + QString("%") + sNote.trimmed() + QString("%") + sPage;
    sendMsg(MSG_NOTE_ADDITEM, t_str);
    m_pDocummentFileHelper->setAnnotationText(ipage, sUuid, sNote);
}

void FileViewWidget::onFileAddNote()
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
        return;

    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE)
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
        notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("Please select the text"));
        return;
    }

    int nPage = _proxy->pointInWhichPage(m_pEndSelectPoint);
    QString msgContent = QString("%1").arg(nPage) + Constant::sQStringSep +
                         QString("%1").arg(m_pEndSelectPoint.x()) + Constant::sQStringSep +
                         QString("%1").arg(m_pEndSelectPoint.y());
    notifyMsg(MSG_OPERATION_TEXT_ADD_ANNOTATION, msgContent);
}

//  文档书签状态改变
void FileViewWidget::slotBookMarkStateChange(int nPage, bool bState)
{
    if (!bState) {
        sendMsg(MSG_OPERATION_DELETE_BOOKMARK, QString("%1").arg(nPage));
    } else {
        sendMsg(MSG_OPERATION_ADD_BOOKMARK, QString("%1").arg(nPage));
    }
    DataManager::instance()->setBIsUpdate(true);
}

//  文档页变化了
void FileViewWidget::slotDocFilePageChanged(int page)
{
//    AppSetting::instance()->setKeyValue(KEY_PAGENUM, QString("%1").arg(page));

    notifyMsg(MSG_FILE_PAGE_CHANGE, QString("%1").arg(page));
}

//  打开文档结果
void FileViewWidget::SlotDocFileOpenResult(bool openresult)
{
    //  通知 其他窗口， 打开文件成功了！！！
    if (openresult) {
        DocummentFileHelper::instance()->setAppShowTitle();
    } else {
        DocummentFileHelper::instance()->setSzFilePath("");
        DataManager::instance()->setStrOnlyFilePath("");
        notifyMsg(MSG_OPERATION_OPEN_FILE_FAIL, tr("Please check if the file is damaged"));
    }
}

//  信号槽　初始化
void FileViewWidget::initConnections()
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            SLOT(slotCustomContextMenuRequested(const QPoint &)));

    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)),
            SLOT(slotDealWithData(const int &, const QString &)));
    connect(this, SIGNAL(sigDealWithKeyMsg(const QString &)),
            SLOT(slotDealWithKeyMsg(const QString &)));
}

void FileViewWidget::__ClickPageLink(Page::Link *pLink)
{
    Page::LinkType_EM linkType = pLink->type;
    if (linkType == Page::LinkType_NULL) {

    } else if (linkType == Page::LinkType_Goto) {
        int page = pLink->page - 1;
        notifyMsg(MSG_DOC_JUMP_PAGE, QString::number(page));
    } else if (linkType == Page::LinkType_GotoOtherFile) {

    } else if (linkType == Page::LinkType_Browse) {
        QString surlOrFileName = pLink->urlOrFileName;
        QDesktopServices::openUrl(QUrl(surlOrFileName, QUrl::TolerantMode));
    } else if (linkType == Page::LinkType_Execute) {

    }
}

//  显示 注释内容Tip
void FileViewWidget::onShowNoteTipWidget(const QPoint &docPos)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (!_proxy)
        return;

    QString selectText, t_strUUid;
    bool bIsHighLightReleasePoint = _proxy->annotationClicked(docPos, selectText, t_strUUid);
    if (bIsHighLightReleasePoint) {
        int nPage = _proxy->pointInWhichPage(docPos);
        QString sContant = "";
        _proxy->getAnnotationText(t_strUUid, sContant, nPage);
        if (sContant != "") {
            if (m_pNoteTipWidget == nullptr) {
                m_pNoteTipWidget = new NoteTipWidget(this);
            }
            m_pNoteTipWidget->setTipContent(sContant);
            QPoint showRealPos(QCursor::pos().x(), QCursor::pos().y() + 10);
            m_pNoteTipWidget->move(showRealPos);
            m_pNoteTipWidget->show();
        }
    }
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
    DocummentProxy *_proxy = DocummentProxy::instance();
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
        notifyMsg(MSG_SELF_ADAPTE_SCALE, QString::number(nScale));
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

    return 0;
}
