#include "DocShowShellWidget.h"
#include <QVBoxLayout>
#include "FileViewWidget.h"
#include "controller/DataManager.h"
#include "DocummentFileHelper.h"
#include <DDialogCloseButton>
#include "utils/PublicFunction.h"
#include "mainShow/FindWidget.h"
#include "pdfControl/fileViewNote/FileViewNoteWidget.h"
#include "application.h"

DocShowShellWidget::DocShowShellWidget(CustomWidget *parent)
    : CustomWidget("DocShowShellWidget", parent)
{
    initWidget();
    initConnections();
}

DocShowShellWidget::~DocShowShellWidget()
{

}

void DocShowShellWidget::resizeEvent(QResizeEvent *event)
{
    int nParentWidth = this->width();

    auto findWidget = this->findChild<FindWidget *>();
    if (findWidget) {
        int nWidget = findWidget->width();
        findWidget->move(nParentWidth - nWidget - 20, 20);
    }


    auto closeBtn = this->findChild<DIconButton *>();
    if (closeBtn) {

        closeBtn->move(nParentWidth - 50, 0);
    }

    CustomWidget::resizeEvent(event);
}

//  全屏 \ 幻灯片放映, 显示 关闭按钮
void DocShowShellWidget::slotShowCloseBtn(const int &iFlag)
{
    auto closeBtn = this->findChild<DIconButton *>();
    if (closeBtn == nullptr) {
        closeBtn = new DIconButton(this);

        connect(closeBtn, &DIconButton::clicked, this, &DocShowShellWidget::slotBtnCloseClicked);

        closeBtn->setIconSize(QSize(50, 50));
        closeBtn->setFixedSize(QSize(50, 50));
    }

    if (iFlag == 1) {
        closeBtn->setIcon(QIcon(":/resources/exit_slider.svg"));
        closeBtn->setObjectName("slider");
    } else {
        QString sIcon = PF::getImagePath("exit_fullscreen", Pri::g_frame);
        closeBtn->setIcon(QIcon(sIcon));
        closeBtn->setObjectName("fullscreen");
    }

    int nParentWidth = this->width();
    closeBtn->move(nParentWidth - 50, 0);

    closeBtn->show();
    closeBtn->raise();
}

void DocShowShellWidget::slotHideCloseBtn()
{
    auto closeBtn = this->findChild<DIconButton *>();
    if (closeBtn != nullptr && closeBtn->isVisible()) {
        closeBtn->hide();
    }
}

//  获取文件的基本数据，　进行展示
void DocShowShellWidget::slotShowFileAttr()
{
    auto pFileAttrWidget = new FileAttrWidget;
    pFileAttrWidget->showScreenCenter();
}

//  搜索框
void DocShowShellWidget::slotShowFindWidget()
{
    auto findWidget = this->findChild<FindWidget *>();
    if (!findWidget) {
        findWidget = new FindWidget(this);
    }

    int nParentWidth = this->width();
    int nWidget = findWidget->width();
    findWidget->move(nParentWidth - nWidget - 20, 20);

    findWidget->show();
    findWidget->raise();
}

//  注释窗口
void DocShowShellWidget::slotOpenNoteWidget(const QString &msgContent)
{
    auto pWidget = this->findChild<FileViewNoteWidget *>();
    if (pWidget == nullptr) {
        pWidget = new FileViewNoteWidget(this);
    }
    pWidget->setEditText("");
    pWidget->setPointAndPage(msgContent);
    pWidget->setNoteUuid("");
    pWidget->setNotePage("");

    QPoint point;
    bool t_bHigh = false;
    DataManager::instance()->setSmallNoteWidgetSize(pWidget->size());
    DataManager::instance()->mousePressLocal(t_bHigh, point);
    pWidget->showWidget(point);
}

//  显示 当前 注释
void DocShowShellWidget::slotShowNoteWidget(const QString &contant)
{
    QStringList t_strList = contant.split(QString("%"), QString::SkipEmptyParts);
    if (t_strList.count() == 3) {
        QString t_strUUid = t_strList.at(0);
        QString t_page = t_strList.at(2);

        QString sContant = "";

        auto pHelper = DocummentFileHelper::instance();
        if (pHelper) {
            pHelper->getAnnotationText(t_strUUid, sContant, t_page.toInt());
        }

        auto pWidget = this->findChild<FileViewNoteWidget *>();
        if (pWidget == nullptr) {
            pWidget = new FileViewNoteWidget(this);
        }
        pWidget->setNoteUuid(t_strUUid);
        pWidget->setNotePage(t_page);
        pWidget->setEditText(sContant);
        pWidget->setPointAndPage("");
        DataManager::instance()->setSmallNoteWidgetSize(pWidget->size());

        bool t_bHigh = false; // 点击位置是否是高亮
        QPoint point;// = this->mapToGlobal(rrect.bottomRight());// 鼠标点击位置

        DataManager::instance()->mousePressLocal(t_bHigh, point);
        if (t_bHigh) {
            pWidget->showWidget(point);
        }
    }
}

void DocShowShellWidget::slotBtnCloseClicked()
{
    notifyMsg(MSG_NOTIFY_KEY_MSG, "Esc");
}

void DocShowShellWidget::slotUpdateTheme()
{
    auto closeBtn = this->findChild<DIconButton *>();
    if (!closeBtn) {
        if (closeBtn->objectName() == "fullscreen") {
            QString sIcon = PF::getImagePath("exit_fullscreen", Pri::g_frame);
            closeBtn->setIcon(QIcon(sIcon));
        }
    }
}

void DocShowShellWidget::initConnections()
{
    connect(this, SIGNAL(sigShowFileAttr()), SLOT(slotShowFileAttr()));
    connect(this, SIGNAL(sigShowFileFind()), SLOT(slotShowFindWidget()));
    connect(this, SIGNAL(sigShowCloseBtn(const int &)), SLOT(slotShowCloseBtn(const int &)));
    connect(this, SIGNAL(sigHideCloseBtn()), SLOT(slotHideCloseBtn()));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));

    connect(this, SIGNAL(sigOpenNoteWidget(const QString &)), SLOT(slotOpenNoteWidget(const QString &)));
    connect(this, SIGNAL(sigShowNoteWidget(const QString &)), SLOT(slotShowNoteWidget(const QString &)));
}

//  集中处理 按键通知消息
int DocShowShellWidget::dealWithNotifyMsg(const QString &msgContent)
{
    if (KeyStr::g_ctrl_f == msgContent) {    //  搜索
        emit sigShowFileFind();
        return ConstantMsg::g_effective_res;
    }

    if (KeyStr::g_esc == msgContent) {   //  退出   幻灯片\全屏
        emit sigHideCloseBtn();
    }
    return 0;
}

int DocShowShellWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPERATION_ATTR:                    //  打开该文件的属性信息
        emit sigShowFileAttr();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_FIND:                    //  搜索
        emit sigShowFileFind();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_ADD_ANNOTATION:     //  添加注释
        emit sigOpenNoteWidget(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_SHOW_NOTEWIDGET:    //  显示注释窗口
        emit sigShowNoteWidget(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SLIDE :                  //  幻灯片模式
        emit sigShowCloseBtn(1);
        break;
    case MSG_OPERATION_FULLSCREEN :             //  全屏
        emit sigShowCloseBtn(0);
        break;
    case MSG_NOTIFY_KEY_MSG : {                 //  最后一个处理通知消息
        return dealWithNotifyMsg(msgContent);
    }
    }

    return 0;
}

void DocShowShellWidget::initWidget()
{
    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(new FileViewWidget);

    this->setLayout(layout);
}
