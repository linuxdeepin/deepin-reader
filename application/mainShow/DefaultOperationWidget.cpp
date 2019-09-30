#include "DefaultOperationWidget.h"
#include "controller/DataManager.h"

#include "docview/docummentproxy.h"
#include "translator/PdfControl.h"
#include "translator/Frame.h"

DefaultOperationWidget::DefaultOperationWidget(CustomWidget *parent)
    : CustomWidget("DefaultOperationWidget", parent)
{
    setWindowFlags(Qt::Popup);

    initWidget();
}

void DefaultOperationWidget::showWidget(const int &x, const int &y)
{
    auto btnList = this->findChildren<DPushButton *>();
    foreach (DPushButton *btn, btnList) {
        if (btn->objectName() == PdfControl::ADD_BK) {
            bool bBookState = DataManager::instance()->bIsBookMarkState();
            btn->setEnabled(!bBookState);
            break;
        }
    }

    this->show();
    this->move(x, y);
    this->raise();
}

void DefaultOperationWidget::createBtn(const QString &btnName, const char *member)
{
    DPushButton *btn = new DPushButton(btnName);
    btn->setObjectName(btnName);
    connect(btn, SIGNAL(clicked()), member);

    layout->addWidget(btn);
}

void DefaultOperationWidget::SlotBtnSearchClicked()
{
    sendMsgAndHide(MSG_OPERATION_FIND);
}

void DefaultOperationWidget::SlotBtnAddBookMarkClicked()
{
    int nCurPage = DocummentProxy::instance()->currentPageNo();
    sendMsgAndHide(MSG_OPERATION_ADD_BOOKMARK, QString("%1").arg(nCurPage));
}

void DefaultOperationWidget::SlotBtnFirstPageClicked()
{
    sendMsgAndHide(MSG_OPERATION_FIRST_PAGE);
}

void DefaultOperationWidget::SlotBtnPrevPageClicked()
{
    sendMsgAndHide(MSG_OPERATION_PREV_PAGE);
}

void DefaultOperationWidget::SlotBtnNextPageClicked()
{
    sendMsgAndHide(MSG_OPERATION_NEXT_PAGE);
}

void DefaultOperationWidget::SlotBtnEndPageClicked()
{
    sendMsgAndHide(MSG_OPERATION_END_PAGE);
}

void DefaultOperationWidget::sendMsgAndHide(const int &msgType, const QString &msgContent)
{
    sendMsg(msgType, msgContent);
    this->hide();
}

int DefaultOperationWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {  //  主题变更

    }
    return 0;
}

void DefaultOperationWidget::initWidget()
{
    layout = new  QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);

    createBtn(Frame::sSearch, SLOT(SlotBtnSearchClicked()));
    createBtn(PdfControl::ADD_BK, SLOT(SlotBtnAddBookMarkClicked()));
    createBtn(Frame::FIRST_PAGE, SLOT(SlotBtnFirstPageClicked()));
    createBtn(Frame::PREV_PAGE, SLOT(SlotBtnPrevPageClicked()));
    createBtn(Frame::NEXT_PAGE, SLOT(SlotBtnNextPageClicked()));
    createBtn(Frame::END_PAGE, SLOT(SlotBtnEndPageClicked()));
}
