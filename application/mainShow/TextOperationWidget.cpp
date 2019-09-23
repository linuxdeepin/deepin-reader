#include "TextOperationWidget.h"
#include "translator/PdfControl.h"
#include "translator/Frame.h"

TextOperationWidget::TextOperationWidget(CustomWidget *parent)
    : CustomWidget("TextOperationWidget", parent)
{
    setWindowFlags(Qt::Popup);

    initWidget();
}

//  显示当前操作框
void TextOperationWidget::showWidget(const int &x, const int &y, const bool &bBookState)
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

DPushButton *TextOperationWidget::createBtn(const QString &btnName, const char *member)
{
    DPushButton *btn = new DPushButton(btnName);
    connect(btn, SIGNAL(clicked()), member);
    return btn;
}

void TextOperationWidget::SlotBtnCopyClicked()
{
    sendMsgAndHide(MSG_OPERATION_TEXT_COPY);
}

void TextOperationWidget::SlotBtnRemoveHighLightedClicked()
{
    sendMsgAndHide(MSG_OPERATION_TEXT_REMOVE_HIGHLIGHTED);
}

void TextOperationWidget::SlotBtnAddAnnotationClicked()
{
    sendMsgAndHide(MSG_OPERATION_TEXT_ADD_ANNOTATION);
}

void TextOperationWidget::SlotBtnAddBookMarkClicked()
{
    sendMsgAndHide(MSG_OPERATION_TEXT_ADD_BOOKMARK);
}

void TextOperationWidget::SlotSendLightedColor(const int &nColor)
{
    sendMsgAndHide(MSG_OPERATION_TEXT_ADD_HIGHLIGHTED, QString::number(nColor));
}

void TextOperationWidget::sendMsgAndHide(const int &msgType, const QString &msgContent)
{
    sendMsg(msgType, msgContent);
    this->hide();
}

int TextOperationWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void TextOperationWidget::initWidget()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);

    DPushButton *copyBtn = createBtn(tr("copy"), SLOT(SlotBtnCopyClicked()));
    layout->addWidget(copyBtn);

    LightedWidget *pLightedWidget = new LightedWidget;
    connect(pLightedWidget, SIGNAL(sigSendLightedColor(const int &)), this, SLOT(SlotSendLightedColor(const int &)));
    layout->addWidget(pLightedWidget);

    DPushButton *removeBtn = createBtn(tr("remove high lighted"), SLOT(SlotBtnRemoveHighLightedClicked()));
    layout->addWidget(removeBtn);
    DPushButton *addAnnoBtn = createBtn(PdfControl::ADD_NOTE, SLOT(SlotBtnAddAnnotationClicked()));
    layout->addWidget(addAnnoBtn);
    DPushButton *AddBookMarkBtn = createBtn(PdfControl::ADD_BK, SLOT(SlotBtnAddBookMarkClicked()));
    layout->addWidget(AddBookMarkBtn);
}
