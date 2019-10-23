#include "DocShowShellWidget.h"
#include <QVBoxLayout>
#include "FileViewWidget.h"

DocShowShellWidget::DocShowShellWidget(CustomWidget *parent)
    : CustomWidget ("DocShowShellWidget", parent)
{
    initWidget();
    initConnections();
}

DocShowShellWidget::~DocShowShellWidget()
{
    if (m_pFileAttrWidget) {
        m_pFileAttrWidget->deleteLater();
        m_pFileAttrWidget = nullptr;
    }
}

void DocShowShellWidget::resizeEvent(QResizeEvent *event)
{
    if (m_pFindWidget != nullptr) {
        int nParentWidth = this->width();
        int nWidget = m_pFindWidget->width();
        m_pFindWidget->move(nParentWidth - nWidget - 20, 20);
    }

    if (m_pFileViewNoteWidget != nullptr) {
        int nParentWidth = this->width();
        int nWidget = m_pFileViewNoteWidget->width();
        m_pFileViewNoteWidget->move(nParentWidth - nWidget - 50, 200);
    }

    setBookMarkStateWidget();

    CustomWidget::resizeEvent(event);
}

//  获取文件的基本数据，　进行展示
void DocShowShellWidget::slotShowFileAttr()
{
    if (m_pFileAttrWidget == nullptr) {
        m_pFileAttrWidget = new FileAttrWidget;
    }
    m_pFileAttrWidget->showScreenCenter();
}

//  搜索框
void DocShowShellWidget::slotShowFindWidget()
{
    if (m_pFindWidget == nullptr) {
        m_pFindWidget = new FindWidget(this);
    }

    int nParentWidth = this->width();
    int nWidget = m_pFindWidget->width();
    m_pFindWidget->move(nParentWidth - nWidget - 20, 20);

    m_pFindWidget->show();
    m_pFindWidget->raise();
}

//  注释窗口
void DocShowShellWidget::slotOpenNoteWidget(const QString &sPoint)
{
    if (m_pFileViewNoteWidget == nullptr) {
        m_pFileViewNoteWidget = new FileViewNoteWidget(this);
    }
    m_pFileViewNoteWidget->setEditText("");

    int nParentWidth = this->width();
    int nWidth = m_pFileViewNoteWidget->width();
    QString contant;

    QStringList ssPointList = sPoint.split(",,,,", QString::SkipEmptyParts);
    if (ssPointList.size() == 2) {  //  修改
        QString sSelectTextUUid = ssPointList.at(1); //  选中添加注释的文字UUid

        auto pDocummentProxy = DocummentProxy::instance();
        QString contant;
        pDocummentProxy->getAnnotationText(sSelectTextUUid, contant);

        m_pFileViewNoteWidget->setEditText(contant);
    }

    m_pFileViewNoteWidget->move(nParentWidth - nWidth - 50, 200);
    m_pFileViewNoteWidget->show();
    m_pFileViewNoteWidget->raise();
}

//  显示 当前 注释
void DocShowShellWidget::slotShowNoteWidget(const QString &contant)
{
    QString t_strUUid;
    int t_nPage = -1;
    bool t_isShow = isShowW(contant, t_nPage, t_strUUid);

    if (!t_isShow) {
        slotCloseNoteWidget();
        return;
    }

    if (m_pFileViewNoteWidget == nullptr) {
        m_pFileViewNoteWidget = new FileViewNoteWidget(this);
    }

    int nParentWidth = this->width();
    int nWidth = m_pFileViewNoteWidget->width();

    QString t_strNote;
    auto pDocummentProxy = DocummentProxy::instance();
    pDocummentProxy->getAnnotationText(t_strUUid, t_strNote, t_nPage);

    m_pFileViewNoteWidget->setEditText(t_strNote);
    m_pFileViewNoteWidget->move(nParentWidth - nWidth - 50, 200);
    m_pFileViewNoteWidget->show();
    m_pFileViewNoteWidget->raise();
}

void DocShowShellWidget::slotCloseNoteWidget()
{
    if (m_pFileViewNoteWidget) {
        m_pFileViewNoteWidget->closeWidget();
    }
}


void DocShowShellWidget::initConnections()
{
    connect(this, SIGNAL(sigShowFileAttr()), this, SLOT(slotShowFileAttr()));
    connect(this, SIGNAL(sigShowFileFind()), this, SLOT(slotShowFindWidget()));

    connect(this, SIGNAL(sigOpenNoteWidget(const QString &)), this, SLOT(slotOpenNoteWidget(const QString &)));
    connect(this, SIGNAL(sigShowNoteWidget(const QString &)), this, SLOT(slotShowNoteWidget(const QString &)));
    connect(this, SIGNAL(sigCloseNoteWidget()), this, SLOT(slotCloseNoteWidget()));
}

int DocShowShellWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPERATION_ATTR:        //  打开该文件的属性信息
        emit sigShowFileAttr();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_FIND:        //  搜索
        emit sigShowFileFind();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_ADD_ANNOTATION:     //  添加注释
        emit sigOpenNoteWidget(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_SHOW_NOTEWIDGET:    //  显示注释窗口
        emit sigShowNoteWidget(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_CLOSE_NOTEWIDGET:   //  关闭注释窗口
        emit sigCloseNoteWidget();
        break;
    case MSG_NOTIFY_KEY_MSG : {    //  最后一个处理通知消息
        if ("Ctrl+F" == msgContent) {
            emit sigShowFileFind();
            return ConstantMsg::g_effective_res;
        }
    }
    case MSG_CLOSE_NOTE_WIDGET: {
        emit sigCloseNoteWidget();
    }
    break;
    }

    return 0;
}

void DocShowShellWidget::initWidget()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(new FileViewWidget);

    this->setLayout(layout);

    setBookMarkStateWidget();
}

//  书签状态
void DocShowShellWidget::setBookMarkStateWidget()
{
    if (m_pBookMarkStateLabel == nullptr) {
        m_pBookMarkStateLabel = new BookMarkStateLabel(this);
    }
    int nParentWidth = this->width();
    int nWidget = m_pBookMarkStateLabel->width();
    m_pBookMarkStateLabel->move(nParentWidth - nWidget - 20, 0);
    m_pBookMarkStateLabel->show();
    m_pBookMarkStateLabel->raise();
}

bool DocShowShellWidget::isShowW(const QString &contant, int &page, QString &uuid)
{
    QStringList t_strList = contant.split(QString("%"), QString::SkipEmptyParts);
    if (t_strList.count() == 3) {
        uuid = t_strList.at(0);
        page = t_strList.at(2).toInt();
        int t_show = t_strList.at(1).toInt();
        if (t_show) {
            return true;
        } else {
            return false;
        }
    } else {
        page = -1;
        uuid = QString("");

        return false;
    }
}
