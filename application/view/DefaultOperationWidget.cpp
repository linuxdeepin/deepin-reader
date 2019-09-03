#include "DefaultOperationWidget.h"

DefaultOperationWidget::DefaultOperationWidget(CustomWidget *parent)
    : CustomWidget(parent)
{
    setWindowFlags(Qt::Popup);

    initWidget();
}

void DefaultOperationWidget::createBtn(const QString &btnName, const char *member)
{
    DPushButton *btn = new DPushButton(btnName);
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

    createBtn(tr("search"), SLOT(SlotBtnSearchClicked()));
    createBtn(tr("add book mark"), SLOT(SlotBtnAddBookMarkClicked()));
    createBtn(tr("first page"), SLOT(SlotBtnFirstPageClicked()));
    createBtn(tr("prev page"), SLOT(SlotBtnPrevPageClicked()));
    createBtn(tr("next page"), SLOT(SlotBtnNextPageClicked()));
    createBtn(tr("end page"), SLOT(SlotBtnEndPageClicked()));
}
