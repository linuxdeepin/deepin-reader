#include "DefaultOperationWidget.h"

#include "docview/docummentproxy.h"
#include "translator/PdfControl.h"
#include "translator/Frame.h"

DefaultOperationWidget::DefaultOperationWidget(CustomWidget *parent)
    : CustomWidget("DefaultOperationWidget", parent)
{
    setWindowFlags(Qt::Popup);

    initWidget();
}

void DefaultOperationWidget::showWidget(const int &x, const int &y, const bool &bBookState)
{
    QList<DPushButton *> btnList = this->findChildren<DPushButton *>();
    foreach (DPushButton *btn, btnList) {
        if (btn->objectName() == PdfControl::ADD_BK) {
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
    sendMsgAndHide(MSG_OPERATION_ADD_BOOKMARK);
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

void DefaultOperationWidget::sendMsgAndHide(const int &msgType)
{
    sendMsg(msgType);
    this->hide();
}

int DefaultOperationWidget::dealWithData(const int &, const QString &)
{
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
