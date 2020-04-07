#include "SheetBrowserPDFPrivate.h"

#include <QDesktopServices>
#include <QFileInfo>

#include "controller/Annotation.h"
#include "controller/ProxyNotifyMsg.h"
#include "controller/ProxyViewDisplay.h"
#include "controller/ProxyMouseMove.h"
#include "controller/ProxyData.h"
#include "controller/ProxyFileDataModel.h"

#include "menu/TextOperationMenu.h"
#include "menu/DefaultOperationMenu.h"

#include "business/FileFormatHelper.h"
#include "widgets/FindWidget.h"
#include "docview/docummentproxy.h"
#include "business/AppInfo.h"
#include "SheetBrowserPDF.h"
#include "widgets/NoteTipWidget.h"


#include "pdfControl/note/NoteViewWidget.h"
#include "CentralDocPage.h"
#include "DocSheet.h"

SheetBrowserPDFPrivate::SheetBrowserPDFPrivate(DocSheet *sheet, SheetBrowserPDF *parent)
    : m_sheet(sheet), q_ptr(parent)
{
    m_pProxyData = new ProxyData(this);
    connect(m_pProxyData, SIGNAL(signale_filechanged(bool)), q_ptr, SIGNAL(sigFileChanged(bool)));

    m_operatemenu = new TextOperationMenu(parent);
    connect(m_operatemenu, SIGNAL(sigActionTrigger(const int &, const QString &)), SLOT(slotDealWithMenu(const int &, const QString &)));

    m_pDefaultMenu = new DefaultOperationMenu(parent);
    connect(m_pDefaultMenu, SIGNAL(sigActionTrigger(const int &, const QString &)), SLOT(slotDealWithMenu(const int &, const QString &)));

    m_pAnnotation = new Annotation(this);
    m_pDocViewProxy = new ProxyViewDisplay(this);
    m_pProxyNotifyMsg = new ProxyNotifyMsg(this);
    m_pProxyMouseMove = new ProxyMouseMove(this);
    m_pProxyFileDataModel = new ProxyFileDataModel(this);
}

SheetBrowserPDFPrivate::~SheetBrowserPDFPrivate()
{
    if (m_pProxy) {
        m_pProxy->closeFile();
    }
}

void SheetBrowserPDFPrivate::hidetipwidget()
{
    if (nullptr != m_pTipWidget && m_pTipWidget->isVisible()) {
        m_pTipWidget->hide();
    }
}

void SheetBrowserPDFPrivate::slotDealWithMenu(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_NOTE_REMOVE_HIGHLIGHT || msgType == MSG_NOTE_UPDATE_HIGHLIGHT_COLOR ||
            msgType == MSG_NOTE_ADD_HIGHLIGHT_COLOR) {                 //  移除高亮注释 的高亮
        m_pAnnotation->dealWithDataMsg(msgType, msgContent);
    } else if (msgType == MSG_OPERATION_TEXT_ADD_ANNOTATION) {  //  添加注释
        onOpenNoteWidget(msgContent);
    } else if (msgType == MSG_OPERATION_TEXT_SHOW_NOTEWIDGET) {
        onShowNoteWidget(msgContent);
    } else if (msgType == MSG_OPERATION_DELETE_BOOKMARK) {      //  右键删除
        onBookMarkState(msgType, msgContent);
    } else if (msgType == MSG_OPERATION_ADD_BOOKMARK) {      //  右键添加
        onBookMarkState(msgType, msgContent);
    }
}

void SheetBrowserPDFPrivate::SlotNoteViewMsg(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_NOTE_ADD_CONTENT) {                      //  新增高亮注释
        AddHighLightAnnotation(msgContent);
    } else if (msgType == MSG_NOTE_DELETE_CONTENT || msgType == MSG_NOTE_UPDATE_CONTENT || msgType == MSG_NOTE_PAGE_ADD_CONTENT
               || msgType == MSG_NOTE_PAGE_UPDATE_CONTENT || msgType == MSG_NOTE_PAGE_DELETE_CONTENT) {
        m_pAnnotation->dealWithDataMsg(msgType, msgContent);
    }
}

//  按 delete 键 删除
void SheetBrowserPDFPrivate::SlotDeleteAnntation(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_NOTE_DELETE_CONTENT || msgType == MSG_NOTE_PAGE_DELETE_CONTENT) {
        m_pAnnotation->dealWithDataMsg(msgType, msgContent);
    }
}

void SheetBrowserPDFPrivate::mouseMoveEvent(QMouseEvent *event)
{
    m_pProxyMouseMove->mouseMoveEvent(event);
}

void SheetBrowserPDFPrivate::mousePressEvent(QMouseEvent *event)
{
    m_pProxyMouseMove->mousePressEvent(event);
}

void SheetBrowserPDFPrivate::mouseReleaseEvent(QMouseEvent *event)
{
    m_pProxyMouseMove->mouseReleaseEvent(event);
}

void SheetBrowserPDFPrivate::AddHighLightAnnotation(const QString &msgContent)
{
    QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (contentList.size() == 2) {
        QString sNote = contentList.at(0);
        QString sPage = contentList.at(1);

        int nSx = m_pProxyData->getStartPoint().x();
        int nSy = m_pProxyData->getStartPoint().y();

        int nEx = m_pProxyData->getEndSelectPoint().x();
        int nEy = m_pProxyData->getEndSelectPoint().y();

        QString sContent = QString::number(nSx) + Constant::sQStringSep +
                           QString::number(nSy) + Constant::sQStringSep +
                           QString::number(nEx) + Constant::sQStringSep +
                           QString::number(nEy) + Constant::sQStringSep +
                           sNote + Constant::sQStringSep +
                           sPage;

        m_pAnnotation->AddHighLightAnnotation(sContent);
    }
}

void SheetBrowserPDFPrivate::OnShortCutKey(const QString &sKey)
{
    //  处于幻灯片模式下
    int nState = CentralDocPage::Instance()->getCurrentState();
    if (nState == SLIDER_SHOW)
        return;

    //  放大镜状态， 直接返回
    if (nState == Magnifer_State)
        return;

    //  手型状态， 直接返回
    if (nState == Handel_State)
        return;

    if (sKey == KeyStr::g_ctrl_c) {
        DocFile_ctrl_c();
    } else if (sKey == KeyStr::g_ctrl_l) {
        DocFile_ctrl_l();
    } else if (sKey == KeyStr::g_ctrl_i) {
        DocFile_ctrl_i();
    }
}

//  添加高亮颜色  快捷键
void SheetBrowserPDFPrivate::DocFile_ctrl_l()
{
    int nSx = m_pProxyData->getStartPoint().x();
    int nSy = m_pProxyData->getStartPoint().y();

    int nEx = m_pProxyData->getEndSelectPoint().x();
    int nEy = m_pProxyData->getEndSelectPoint().y();

    if (nSx == nEx && nSy == nEy) {
        m_sheet->showTips(tr("Please select the text"));
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

        m_pAnnotation->AddHighLight(sContent);
    } else {
        m_sheet->showTips(tr("Please select the text"));
    }
}

void SheetBrowserPDFPrivate::DocFile_ctrl_i()
{
    if (m_pProxy) {
        QString selectText;
        if (m_pProxy->getSelectTextString(selectText)) {

            int nSx = m_pProxyData->getStartPoint().x();
            int nSy = m_pProxyData->getStartPoint().y();

            int nEx = m_pProxyData->getEndSelectPoint().x();
            int nEy = m_pProxyData->getEndSelectPoint().y();

            if ((nSx == nEx && nSy == nEy)) {
                m_sheet->showTips(tr("Please select the text"));
                return;
            }

            int nPage = m_pProxy->pointInWhichPage(m_pProxyData->getEndSelectPoint());
            QString msgContent = QString("%1").arg(nPage) + Constant::sQStringSep +
                                 QString("%1").arg(nEx) + Constant::sQStringSep +
                                 QString("%1").arg(nEy);

            onOpenNoteWidget(msgContent);
        } else {
            m_sheet->showTips(tr("Please select the text"));
        }
    }
}

void SheetBrowserPDFPrivate::DocFile_ctrl_c()
{
    if (m_pProxy) {
        QString sSelectText = "";
        if (m_pProxy->getSelectTextString(sSelectText)) { //  选择　当前选中下面是否有文字
            Utils::copyText(sSelectText);
        }
    }
}

int SheetBrowserPDFPrivate::qDealWithShortKey(const QString &sKey)
{
    QList<QString> KeyMsgList = {KeyStr::g_ctrl_l, KeyStr::g_ctrl_i, KeyStr::g_ctrl_c};

    if (KeyMsgList.contains(sKey)) {
        OnShortCutKey(sKey);
        return MSG_OK;
    }
    return MSG_NO_OK;
}

//  消息 数据 处理
int SheetBrowserPDFPrivate::dealWithData(const int &msgType, const QString &msgContent)
{
    QList<int> msgList = { MSG_HANDLESHAPE,
                           MSG_VIEWCHANGE_DOUBLE_SHOW, MSG_VIEWCHANGE_ROTATE, MSG_FILE_SCALE, MSG_VIEWCHANGE_FIT,
                           MSG_OPERATION_TEXT_ADD_ANNOTATION,
                           MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, MSG_NOTE_PAGE_SHOW_NOTEWIDGET
                         };

    Q_Q(SheetBrowserPDF);

    if (q->m_pFindWidget) {
        int nRes = q->m_pFindWidget->dealWithData(msgType, msgContent);
        if (nRes == MSG_OK) {
            return nRes;
        }
    }

    //  数据变化
    m_pProxyFileDataModel->qDealWithData(msgType, msgContent);

    if (msgType == MSG_VIEWCHANGE_DOUBLE_SHOW) {
        m_pDocViewProxy->OnSetViewChange(msgContent);
        handleResize(q->size());
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

    bool rl = msgList.contains(msgType);
    if (rl) {
        return MSG_OK;
    }

    return MSG_NO_OK;
}

//  手势控制
void SheetBrowserPDFPrivate::onSetHandShape(const QString &data)
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
    CentralDocPage::Instance()->setCurrentState(nRes);
}

void SheetBrowserPDFPrivate::showNoteViewWidget(const QString &sPage, const QString &t_strUUid, const QString &sText, const int &nType)
{
    Q_Q(SheetBrowserPDF);

    if (m_pNoteViewWidget == nullptr) {
        m_pNoteViewWidget = new NoteViewWidget(q);
        connect(m_pNoteViewWidget, SIGNAL(sigNoteViewMsg(const int &, const QString &)), SLOT(SlotNoteViewMsg(const int &, const QString &)));
        connect(m_pNoteViewWidget, SIGNAL(sigNeedShowTips(const QString &)), m_sheet, SLOT(onShowTips(const QString &)));
    }
    m_pNoteViewWidget->setEditText(sText);
    m_pNoteViewWidget->setNoteUuid(t_strUUid);
    m_pNoteViewWidget->setNotePage(sPage);
    m_pNoteViewWidget->setWidgetType(nType);

    QPoint point;
    bool t_bHigh = false;
    dApp->m_pAppInfo->setSmallNoteWidgetSize(m_pNoteViewWidget->size());
    dApp->m_pAppInfo->mousePressLocal(t_bHigh, point);
    m_pNoteViewWidget->showWidget(point);
}

void SheetBrowserPDFPrivate::__SetCursor(const QCursor &cs)
{
    Q_Q(SheetBrowserPDF);
    const QCursor ss = q->cursor();    //  当前鼠标状态
    if (ss != cs) {
        q->setCursor(cs);
    }
}


void SheetBrowserPDFPrivate::FindOperation(const int &iType, const QString &strFind)
{
    if (iType == E_FIND_CONTENT || iType == E_FIND_EXIT) {
        notifyMsg(iType, m_pProxyData->getPath());
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

void SheetBrowserPDFPrivate::handleResize(const QSize &size)
{
    Q_Q(SheetBrowserPDF);

    if (!m_pProxyData->IsFirstShow() && m_pProxyData->getIsFileOpenOk()) {

        m_pDocViewProxy->setWidth(size.width());
        m_pDocViewProxy->setHeight(size.height());
        m_pDocViewProxy->onSetWidgetAdapt();
    }

    if (q->m_pFindWidget && q->m_pFindWidget->isVisible()) {
        int nParentWidth = q->width();
        q->m_pFindWidget->showPosition(nParentWidth);
    }
}

void SheetBrowserPDFPrivate::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
    handleResize(size);
}

void SheetBrowserPDFPrivate::wheelEvent(QWheelEvent *event)
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
}

void SheetBrowserPDFPrivate::onOpenNoteWidget(const QString &msgContent)
{
    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 3) {

        QString sPage = sList.at(0);
        showNoteViewWidget(sPage);
    }
}

//  显示 当前 注释
void SheetBrowserPDFPrivate::onShowNoteWidget(const QString &contant)
{
    QStringList t_strList = contant.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (t_strList.count() == 2) {
        QString t_strUUid = t_strList.at(0);
        QString t_page = t_strList.at(1);

        QString sContant = "";

        m_pProxy->getAnnotationText(t_strUUid, sContant, t_page.toInt());

        showNoteViewWidget(t_page, t_strUUid, sContant);
    }
}

void SheetBrowserPDFPrivate::__ShowPageNoteWidget(const QString &msgContent)
{
    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 4) {
        QString sUuid = sList.at(0);
        QString sPage = sList.at(1);
        QString sX = sList.at(2);
        QString sY = sList.at(3);

        QString sContant = "";

        m_pProxy->getAnnotationText(sUuid, sContant, sPage.toInt());

        showNoteViewWidget(sPage, sUuid, sContant, NOTE_ICON);
    }
}

void SheetBrowserPDFPrivate::__ShowNoteTipWidget(const QString &sText)
{
    Q_Q(SheetBrowserPDF);
    if (m_pTipWidget == nullptr) {
        m_pTipWidget = new NoteTipWidget(q);
    }
    m_pTipWidget->setTipContent(sText);
    QPoint showRealPos(QCursor::pos().x(), QCursor::pos().y() + 10);
    m_pTipWidget->move(showRealPos);
    m_pTipWidget->show();
}


void SheetBrowserPDFPrivate::__CloseFileNoteWidget()
{
    if (m_pTipWidget && m_pTipWidget->isVisible()) {
        m_pTipWidget->close();
    }
}

void SheetBrowserPDFPrivate::slotCustomContextMenuRequested(const QPoint &point)
{
    Q_Q(SheetBrowserPDF);
    //  处于幻灯片模式下
    int nState = CentralDocPage::Instance()->getCurrentState();
    if (nState == SLIDER_SHOW)
        return;

    //  放大镜状态， 直接返回
    if (nState == Magnifer_State)
        return;

    //  手型状态， 直接返回
    if (nState == Handel_State)
        return;

    QString sSelectText = "";

    int textPage = 0;

    m_pProxy->getSelectTextString(sSelectText, textPage); //  选择　当前选中下面是否有文字

    QPoint tempPoint = q->mapToGlobal(point);

    m_popwidgetshowpoint = tempPoint;

    QPoint pRightClickPoint = m_pProxy->global2RelativePoint(tempPoint);

    //  右键鼠标点 是否有高亮区域
    QString sAnnotationText = "", struuid = "";

    bool bIsHighLight = m_pProxy->annotationClicked(pRightClickPoint, sAnnotationText, struuid);

    bool bicon = m_pProxy->iconAnnotationClicked(pRightClickPoint, sAnnotationText, struuid);

    int clickPage = m_pProxy->pointInWhichPage(pRightClickPoint);

    if (sSelectText != "") {
        m_operatemenu->setClickPoint(pRightClickPoint);

        m_operatemenu->setPStartPoint(m_pProxyData->getStartPoint());

        m_operatemenu->setPEndPoint(m_pProxyData->getEndSelectPoint());

        m_operatemenu->setClickPage(textPage);

        dApp->m_pAppInfo->setMousePressLocal(bIsHighLight, tempPoint);

        bool bremoveenable = false;

        if (bicon) {
            m_operatemenu->setType(NOTE_ICON);
        } else {
            m_operatemenu->setType(NOTE_HIGHLIGHT);
        }

        if (bicon || bIsHighLight)
            bremoveenable = true;

        m_operatemenu->setRemoveEnabled(bremoveenable);

        m_operatemenu->execMenu(tempPoint, true, sSelectText, struuid);

    } else if (sSelectText == "" && (bIsHighLight || bicon)) { //  选中区域 有文字, 弹出 文字操作菜单
        //  需要　区别　当前选中的区域，　弹出　不一样的　菜单选项
        m_operatemenu->setClickPoint(pRightClickPoint);

        m_operatemenu->setPStartPoint(m_pProxyData->getStartPoint());

        m_operatemenu->setPEndPoint(m_pProxyData->getEndSelectPoint());

        m_operatemenu->setClickPage(clickPage);

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
        dApp->m_pAppInfo->setMousePressLocal(false, tempPoint);

        m_pDefaultMenu->setClickpoint(pRightClickPoint);

        m_pDefaultMenu->setDoubleShow(m_pDocViewProxy->isDoubleShow());
        m_pDefaultMenu->execMenu(tempPoint, clickPage);
    }
}

void SheetBrowserPDFPrivate::SlotDocFileOpenResult(bool openresult)
{
    Q_Q(SheetBrowserPDF);
    //  通知 其他窗口， 打开文件成功了！！！
    if (openresult) {

        dApp->m_pDBService->qSelectData(m_pProxyData->getPath(), DB_BOOKMARK);

        m_pProxyData->setFirstShow(false);
        m_pProxyData->setIsFileOpenOk(true);
    } else {
        m_sheet->showTips(tr("Please check if the file is damaged"));
    }

    emit q->sigFileOpenResult(m_pProxyData->getPath(), openresult);
}

void SheetBrowserPDFPrivate::OpenFilePath(const QString &sPath)
{
    Q_Q(SheetBrowserPDF);
    //  实际文档类  唯一实例化设置 父窗口
    m_pProxy = new DocummentProxy(q);
    if (m_pProxy) {
        connect(m_pProxy, SIGNAL(signal_bookMarkStateChange(int, bool)), m_pProxyNotifyMsg, SLOT(slotBookMarkStateChange(int, bool)));
        connect(m_pProxy, SIGNAL(signal_pageChange(int)), m_pProxyNotifyMsg, SLOT(slotDocFilePageChanged(int)));
        connect(m_pProxy, SIGNAL(signal_openResult(bool)), SLOT(SlotDocFileOpenResult(bool)));

        QFileInfo info(sPath);

        QString sCompleteSuffix = info.completeSuffix();
        DocType_EM nCurDocType = FFH::setCurDocuType(sCompleteSuffix);

        //从数据库中获取文件的字号信息
        dApp->m_pDBService->qSelectData(sPath, DB_HISTROY);

        FileDataModel fdm = dApp->m_pDBService->getHistroyData(sPath);

        m_pProxyFileDataModel->qSetFileData(fdm);

        int nAdapteState = static_cast<int>(fdm.qGetData(Fit));
        m_pDocViewProxy->setAdapteState(nAdapteState);

        int curPage = static_cast<int>(fdm.qGetData(CurPage));
        int iscale  = static_cast<int>(fdm.qGetData(Scale));         // 缩放
        m_pDocViewProxy->setScale(iscale);

        int doubPage = static_cast<int>(fdm.qGetData(DoubleShow));   // 是否是双页
        m_pDocViewProxy->setDoubleShow(doubPage);

        int rotate = static_cast<int>(fdm.qGetData(Rotate));         // 文档旋转角度(0,1,2,3,4)
        m_pDocViewProxy->setRotateType(rotate);

        iscale = (iscale > 500 ? 500 : iscale) <= 0 ? 100 : iscale;
        double scale = iscale / 100.0;
        RotateType_EM rotatetype = static_cast<RotateType_EM>(rotate);
        ViewMode_EM viewmode = static_cast<ViewMode_EM>(doubPage);

        m_pProxyData->setPath(sPath);
        bool rl = m_pProxy->openFile(nCurDocType, sPath, static_cast<unsigned int>(curPage), rotatetype, scale, viewmode);
        if (rl) {
            m_pProxy->setViewFocus();
        }
    }
}

void SheetBrowserPDFPrivate::notifyMsg(const int &msgType, const QString &msgContent)
{
    Q_Q(SheetBrowserPDF);
    q->notifyMsg(msgType, msgContent);
}

void SheetBrowserPDFPrivate::onBookMarkState(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_ADD_BOOKMARK) {
        m_pProxy->setBookMarkState(msgContent.toInt(), true);
    } else if (msgType == MSG_OPERATION_DELETE_BOOKMARK) {
        m_pProxy->setBookMarkState(msgContent.toInt(), false);
    }

    Q_Q(SheetBrowserPDF);
    emit q->sigBookMarkMsg(msgType, msgContent);
}
