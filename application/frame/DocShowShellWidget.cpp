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

    int nParentWidth = this->width();
    int nWidth = m_pFileViewNoteWidget->width();

    QStringList ssPointList = sPoint.split(",,,,", QString::SkipEmptyParts);

    QString sSelectText = ssPointList.at(1); //  选中添加注释的文字

    m_pFileViewNoteWidget->setEditText(QString(""));
    m_pFileViewNoteWidget->move(nParentWidth - nWidth - 50, 200);
    m_pFileViewNoteWidget->show();
    m_pFileViewNoteWidget->raise();
}

void DocShowShellWidget::slotShowNoteWidget(const QString &uuid)
{
    auto pDocummentProxy = DocummentProxy::instance();
    if (m_pFileViewNoteWidget == nullptr) {
        m_pFileViewNoteWidget = new FileViewNoteWidget(this);
    }

    int nParentWidth = this->width();
    int nWidth = m_pFileViewNoteWidget->width();

    QString t_strNote;

    pDocummentProxy->getAnnotationText(uuid, t_strNote);
    qDebug() << "slotShowNoteWidget uuid:" << uuid <<  "    t_strNote:" << t_strNote;

    m_pFileViewNoteWidget->setEditText(t_strNote);
    m_pFileViewNoteWidget->move(nParentWidth - nWidth - 50, 200);
    m_pFileViewNoteWidget->show();
    m_pFileViewNoteWidget->raise();
}


void DocShowShellWidget::initConnections()
{
    connect(this, SIGNAL(sigShowFileAttr()), this, SLOT(slotShowFileAttr()));
    connect(this, SIGNAL(sigShowFileFind()), this, SLOT(slotShowFindWidget()));

    connect(this, SIGNAL(sigOpenNoteWidget(const QString &)), this, SLOT(slotOpenNoteWidget(const QString &)));
    connect(this, SIGNAL(sigShowNoteWidget(const QString &)), this, SLOT(slotShowNoteWidget(const QString &)));
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
    case MSG_NOTIFY_KEY_MSG : {    //  最后一个处理通知消息
        if ("Ctrl+F" == msgContent) {
            emit sigShowFileFind();
            return ConstantMsg::g_effective_res;
        }
    }
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
