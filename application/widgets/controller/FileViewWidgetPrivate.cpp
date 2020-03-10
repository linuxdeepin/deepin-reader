#include "FileViewWidgetPrivate.h"

#include <QDesktopServices>

#include "application.h"
#include "Annotation.h"
#include "docview/docummentproxy.h"
#include "business/AppInfo.h"
#include "widgets/FileViewWidget.h"
#include "widgets/NoteTipWidget.h"

#include "gof/bridge/IHelper.h"
#include "widgets/main/MainTabWidgetEx.h"

FileViewWidgetPrivate::FileViewWidgetPrivate(FileViewWidget *parent)
    : q_ptr(parent)
{
    m_pAnnotation = new Annotation(this);
}

void FileViewWidgetPrivate::slotDealWithMenu(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_NOTE_REMOVE_HIGHLIGHT) {                 //  移除高亮注释 的高亮
        RemoveHighLight(msgContent);
    } else if (msgType == MSG_NOTE_UPDATE_HIGHLIGHT_COLOR) {    //  更新高亮颜色
        ChangeAnnotationColor(msgContent);
    } else if (msgType == MSG_NOTE_ADD_HIGHLIGHT_COLOR) {       //  添加高亮
        AddHighLight(msgContent);
    } else if (msgType == MSG_OPERATION_TEXT_ADD_ANNOTATION) {  //  添加注释
        Q_Q(FileViewWidget);
        q->onOpenNoteWidget(msgContent);
    } else if (msgType == MSG_OPERATION_TEXT_SHOW_NOTEWIDGET) {
        Q_Q(FileViewWidget);
        q->onShowNoteWidget(msgContent);
    }
}

void FileViewWidgetPrivate::SlotNoteViewMsg(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_NOTE_ADD_CONTENT) {                      //  新增高亮注释
        AddHighLightAnnotation(msgContent);
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

//  按 delete 键 删除
void FileViewWidgetPrivate::SlotDeleteAnntation(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_NOTE_DELETE_CONTENT) {
        RemoveAnnotation(msgContent);
    } else if (msgType == MSG_NOTE_PAGE_DELETE_CONTENT) {
        DeletePageIconAnnotation(msgContent);
    }
}

void FileViewWidgetPrivate::setProxy(DocummentProxy *proxy)
{
    m_pAnnotation->setProxy(proxy);
}

void FileViewWidgetPrivate::mouseMoveEvent(QMouseEvent *event)
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
void FileViewWidgetPrivate::__ShowMagnifier(const QPoint &clickPoint)
{
    Q_Q(FileViewWidget);

    QPoint docGlobalPos = q->m_pProxy->global2RelativePoint(clickPoint);

    q->m_pProxy->showMagnifier(docGlobalPos);
    q->__SetCursor(Qt::BlankCursor);
}

//  鼠标按住, 拖动页面
void FileViewWidgetPrivate::__FilePageMove(const QPoint &endPos)
{
    Q_Q(FileViewWidget);

    QPoint mvPoint = m_pMoveStartPoint - endPos;
    int mvX = mvPoint.x();
    int mvY = mvPoint.y();

    q->m_pProxy->pageMove(mvX, mvY);

    q->__SetCursor(QCursor(Qt::OpenHandCursor));

    m_pMoveStartPoint = endPos;
}

//  选中文本
void FileViewWidgetPrivate::__MouseSelectText(const QPoint &clickPoint)
{
    Q_Q(FileViewWidget);

    QPoint docGlobalPos = q->m_pProxy->global2RelativePoint(clickPoint);

    q->m_pEndSelectPoint = docGlobalPos;
    q->m_pProxy->mouseSelectText(q->m_pStartPoint, q->m_pEndSelectPoint);
}

//  其余 鼠标移动
void FileViewWidgetPrivate::__OtherMouseMove(const QPoint &globalPos)
{
    Q_Q(FileViewWidget);

    QPoint docGlobalPos = q->m_pProxy->global2RelativePoint(globalPos);

    //  首先判断文档划过属性
    auto pLink = q->m_pProxy->mouseBeOverLink(docGlobalPos);
    if (pLink) {
        q->__SetCursor(QCursor(Qt::PointingHandCursor));
    } else {
        //  先判断是否是页面注释图标
        if (q->m_pProxy->mouseovericonAnnotation(docGlobalPos)) {
            __ShowPageNoteWidget(docGlobalPos);
        } else if (q->m_pProxy->mouseBeOverText(docGlobalPos)) {
            m_bIsHandleSelect = true;

            q->__SetCursor(QCursor(Qt::IBeamCursor));

            __ShowFileNoteWidget(docGlobalPos);
        } else {
            m_bIsHandleSelect = false;

            q->__SetCursor(QCursor(Qt::ArrowCursor));

            q->__CloseFileNoteWidget();
        }
    }
}

//  显示页面注释 提示界面
void FileViewWidgetPrivate::__ShowPageNoteWidget(const QPoint &docPos)
{
    Q_Q(FileViewWidget);

    QString sText = "", sUuid = "";
    bool isOk = q->m_pProxy->iconAnnotationClicked(docPos, sText, sUuid);
    if (isOk) {
        if (sText != "") {
            q->__ShowNoteTipWidget(sText);
        }
    }
}

//  显示 高亮注释 提示界面
void FileViewWidgetPrivate::__ShowFileNoteWidget(const QPoint &docPos)
{
    Q_Q(FileViewWidget);

    QString selectText, t_strUUid;
    bool bIsHighLightReleasePoint = q->m_pProxy->annotationClicked(docPos, selectText, t_strUUid);
    if (bIsHighLightReleasePoint) {
        int nPage = q->m_pProxy->pointInWhichPage(docPos);
        QString sText = "";
        q->m_pProxy->getAnnotationText(t_strUUid, sText, nPage);
        if (sText != "") {
            q->__ShowNoteTipWidget(sText);
        }
    }
}

void FileViewWidgetPrivate::mousePressEvent(QMouseEvent *event)
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
void FileViewWidgetPrivate::__AddIconAnnotation(const QPoint &globalPos)
{
    Q_Q(FileViewWidget);

    QPoint docGlobalPos = q->m_pProxy->global2RelativePoint(globalPos);

    //  添加之前先判断, 点击处是否 注释图标
    bool bIsIcon = q->m_pProxy->mouseovericonAnnotation(docGlobalPos);
    if (bIsIcon) {
        QString sContent = "", sUuid = "";
        if (q->m_pProxy->iconAnnotationClicked(docGlobalPos, sContent, sUuid)) {
        }
    } else {
        QString sUuid = q->m_pProxy->addIconAnnotation(docGlobalPos);        //  添加注释图标成功
        if (sUuid != "") {
            int nClickPage = q->m_pProxy->pointInWhichPage(docGlobalPos);
            QString strContent = sUuid.trimmed() + Constant::sQStringSep +
                                 QString::number(nClickPage) + Constant::sQStringSep +
                                 QString::number(globalPos.x()) + Constant::sQStringSep +
                                 QString::number(globalPos.y());
            MainTabWidgetEx::Instance()->setCurrentState(Default_State);

            QJsonObject obj;
            obj.insert("content", strContent);
            obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

            QJsonDocument doc(obj);

            dApp->m_pModelService->notifyMsg(MSG_NOTE_PAGE_SHOW_NOTEWIDGET, doc.toJson(QJsonDocument::Compact));
        } else {
            qWarning() << __FUNCTION__ << "          " << sUuid;;
        }
    }
}

void FileViewWidgetPrivate::__HandlClicked(const QPoint &globalPos)
{
    m_pMoveStartPoint = globalPos;  //  变成手，　需要的是　相对坐标
    m_bSelectOrMove = true;
}

void FileViewWidgetPrivate::__OtherMousePress(const QPoint &globalPos)
{
    Q_Q(FileViewWidget);

    QPoint docGlobalPos = q->m_pProxy->global2RelativePoint(globalPos);

    //  点击的时候　先判断　点击处　　是否有链接之类
    auto pLink = q->m_pProxy->mouseBeOverLink(docGlobalPos);
    if (pLink) {
        __ClickPageLink(pLink);
    } else {
        int nState = MainTabWidgetEx::Instance()->getCurrentState();
        if (nState == Default_State) {
            q->m_pProxy->mouseSelectTextClear();  //  清除之前选中的文字高亮

            q->__CloseFileNoteWidget();

            if (m_bIsHandleSelect) {
                m_bSelectOrMove = true;
                q->m_pStartPoint = docGlobalPos;
                q->m_pEndSelectPoint = q->m_pStartPoint;
            }
        }
    }
}

//  点击 页面能够跳转
void FileViewWidgetPrivate::__ClickPageLink(Page::Link *pLink)
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

void FileViewWidgetPrivate::mouseReleaseEvent(QMouseEvent *event)
{
    //  幻灯片模式下, 左键单击 不作任何处理
    int nState = MainTabWidgetEx::Instance()->getCurrentState();
    if (nState == SLIDER_SHOW)
        return;

    Q_Q(FileViewWidget);

    Qt::MouseButton nBtn = event->button();

    QPoint globalPos = event->globalPos();
    QPoint docGlobalPos = q->m_pProxy->global2RelativePoint(globalPos);
    QString selectText, t_strUUid;
    bool bicon = q->m_pProxy->iconAnnotationClicked(docGlobalPos, selectText, t_strUUid);
    //  放大镜状态， 右键则取消放大镜 并且 直接返回
    if (m_bSelectOrMove && !bicon) {
        //判断鼠标左键松开的位置有没有高亮

        dApp->m_pAppInfo->setMousePressLocal(false, globalPos);
        //添加其实结束point是否为同一个，不是同一个说明不是点击可能是选择文字
        if (nBtn == Qt::LeftButton && docGlobalPos == q->m_pStartPoint) {
            // 判断鼠标点击的地方是否有高亮
            bool bIsHighLightReleasePoint = q->m_pProxy->annotationClicked(docGlobalPos, selectText, t_strUUid);

            dApp->m_pAppInfo->setMousePressLocal(bIsHighLightReleasePoint, globalPos);
            if (bIsHighLightReleasePoint) {
                q->__CloseFileNoteWidget();
                int nPage = q->m_pProxy->pointInWhichPage(docGlobalPos);
                QString msgContent = t_strUUid.trimmed() + Constant::sQStringSep + QString::number(nPage);

                QJsonObject obj;
                obj.insert("content", msgContent);
                obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

                QJsonDocument doc(obj);
                q->notifyMsg(MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, doc.toJson(QJsonDocument::Compact));
            }
        }
    } else if (bicon) {
        q->__CloseFileNoteWidget();

        int nPage = q->m_pProxy->pointInWhichPage(docGlobalPos);
        QString strContent = t_strUUid.trimmed() + Constant::sQStringSep + QString::number(nPage) + Constant::sQStringSep +
                             QString::number(globalPos.x()) + Constant::sQStringSep +
                             QString::number(globalPos.y());

        QJsonObject obj;
        obj.insert("content", strContent);
        obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

        QJsonDocument doc(obj);

        dApp->m_pModelService->notifyMsg(MSG_NOTE_PAGE_SHOW_NOTEWIDGET, doc.toJson(QJsonDocument::Compact));
    }

    m_bSelectOrMove = false;
}

void FileViewWidgetPrivate::AddHighLight(const QString &msgContent)
{
    m_pAnnotation->AddHighLight(msgContent);
    m_filechanged = true;
}

void FileViewWidgetPrivate::AddHighLightAnnotation(const QString &msgContent)
{
    Q_Q(FileViewWidget);
    QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (contentList.size() == 2) {
        QString sNote = contentList.at(0);
        QString sPage = contentList.at(1);

        int nSx = q->m_pStartPoint.x();
        int nSy = q->m_pStartPoint.y();

        int nEx = q->m_pEndSelectPoint.x();
        int nEy = q->m_pEndSelectPoint.y();

        QString sContent = QString::number(nSx) + Constant::sQStringSep +
                           QString::number(nSy) + Constant::sQStringSep +
                           QString::number(nEx) + Constant::sQStringSep +
                           QString::number(nEy) + Constant::sQStringSep +
                           sNote + Constant::sQStringSep +
                           sPage;

        QString sRes = m_pAnnotation->AddHighLightAnnotation(sContent);
        if (sRes != "") {
            emit q->sigAnntationMsg(MSG_NOTE_ADD_ITEM, sRes);
            m_filechanged = true;
        }
    }
}

void FileViewWidgetPrivate::RemoveHighLight(const QString &msgContent)
{
    QString sRes = m_pAnnotation->RemoveHighLight(msgContent);
    if (sRes != "") {
        Q_Q(FileViewWidget);
        emit q->sigAnntationMsg(MSG_NOTE_DELETE_ITEM, sRes);
        m_filechanged = true;
    }
}

void FileViewWidgetPrivate::ChangeAnnotationColor(const QString &msgContent)
{
    m_pAnnotation->ChangeAnnotationColor(msgContent);
    m_filechanged = true;
}

void FileViewWidgetPrivate::RemoveAnnotation(const QString &msgContent)
{
    QString sRes = m_pAnnotation->RemoveAnnotation(msgContent);
    if (sRes != "") {
        Q_Q(FileViewWidget);
        emit q->sigAnntationMsg(MSG_NOTE_DELETE_ITEM, sRes);
        m_filechanged = true;
    }
}

void FileViewWidgetPrivate::UpdateAnnotationText(const QString &msgContent)
{
    m_pAnnotation->UpdateAnnotationText(msgContent);
    Q_Q(FileViewWidget);
    emit q->sigAnntationMsg(MSG_NOTE_UPDATE_ITEM, msgContent);
    m_filechanged = true;
}

void FileViewWidgetPrivate::AddPageIconAnnotation(const QString &msgContent)
{
    m_pAnnotation->AddPageIconAnnotation(msgContent);
    Q_Q(FileViewWidget);
    emit q->sigAnntationMsg(MSG_NOTE_PAGE_ADD_ITEM, msgContent);
    m_filechanged = true;
}

void FileViewWidgetPrivate::DeletePageIconAnnotation(const QString &msgContent)
{
    QString sUuid = m_pAnnotation->DeletePageIconAnnotation(msgContent);
    if (sUuid != "") {
        Q_Q(FileViewWidget);
        emit q->sigAnntationMsg(MSG_NOTE_PAGE_DELETE_ITEM, sUuid);
        m_filechanged = true;
    }
}

void FileViewWidgetPrivate::UpdatePageIconAnnotation(const QString &msgContent)
{
    QString sRes = m_pAnnotation->UpdatePageIconAnnotation(msgContent);
    if (sRes != "") {
        Q_Q(FileViewWidget);
        emit q->sigAnntationMsg(MSG_NOTE_PAGE_UPDATE_ITEM, msgContent);

        m_filechanged = true;
    }
}
