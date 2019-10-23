#include "TextOperationWidget.h"
#include "translator/PdfControl.h"
#include "translator/Frame.h"
#include "translator/MainShow.h"
#include "docview/docummentproxy.h"

#include "controller/DataManager.h"

TextOperationWidget::TextOperationWidget(CustomWidget *parent)
    : CustomWidget("TextOperationWidget", parent)
{
    setWindowFlags(Qt::Popup);

    initWidget();
}

//  显示当前操作框
void TextOperationWidget::showWidget(const int &x, const int &y, const bool &bHigh, const QString &sSelectText, const QString &sUuid)
{
    auto btnList = this->findChildren<DPushButton *>();
    foreach (DPushButton *btn, btnList) {
        if (btn->objectName() == PdfControl::ADD_BK) {
            bool bBookState = DataManager::instance()->bIsBookMarkState();
            btn->setEnabled(!bBookState);
        } else if (btn->objectName() == MainShow::REMOVE_HIGH_LIGHTED) {
            btn->setEnabled(bHigh);
        }
    }

    m_nShowY = y;

    m_strSelectText = sSelectText;
    m_strAnnotationUuid = sUuid;

    this->show();
    this->move(x, y);
    this->raise();
}

DPushButton *TextOperationWidget::createBtn(const QString &btnName, const char *member)
{
    auto btn = new DPushButton(btnName);
    btn->setObjectName(btnName);
    connect(btn, SIGNAL(clicked()), member);
    return btn;
}

void TextOperationWidget::SlotBtnCopyClicked()
{
    sendMsgAndHide(MSG_OPERATION_TEXT_COPY);
}

void TextOperationWidget::SlotBtnRemoveHighLightedClicked()
{
    emit sigRemoveHighLighted();
    sendMsgAndHide(MSG_OPERATION_TEXT_REMOVE_HIGHLIGHTED);
}

void TextOperationWidget::SlotBtnAddAnnotationClicked()
{
    sendMsgAndHide(MSG_OPERATION_TEXT_ADD_ANNOTATION, QString("%1,,,,%2")
                   .arg(m_nShowY).arg(m_strAnnotationUuid));
}

void TextOperationWidget::SlotBtnAddBookMarkClicked()
{
    int nCurPage = DocummentProxy::instance()->currentPageNo();
    sendMsgAndHide(MSG_OPERATION_TEXT_ADD_BOOKMARK, QString("%1").arg(nCurPage));
}

//  添加了高亮之后,  移除高亮按钮 需要 能够点击
void TextOperationWidget::SlotSetBtnRemoveHighLightedAble()
{
    auto btnList = this->findChildren<DPushButton *>();
    foreach (DPushButton *btn, btnList) {
        if (btn->objectName() == MainShow::REMOVE_HIGH_LIGHTED) {
            btn->setEnabled(true);
            break;
        }
    }
}

void TextOperationWidget::sendMsgAndHide(const int &msgType, const QString &msgContent)
{
    sendMsg(msgType, msgContent);
    this->hide();
}

int TextOperationWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {  //  主题变更

    }
    return 0;
}

void TextOperationWidget::initWidget()
{
    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);

    auto copyBtn = createBtn(MainShow::COPY, SLOT(SlotBtnCopyClicked()));
    layout->addWidget(copyBtn);

    auto pLightedWidget = new LightedWidget;
    connect(pLightedWidget, SIGNAL(sigAddHighLighted()), this, SLOT(SlotSetBtnRemoveHighLightedAble()));
    connect(this, SIGNAL(sigRemoveHighLighted()), pLightedWidget, SLOT(SlotRemoveHighLighted()));
    layout->addWidget(pLightedWidget);

    auto removeBtn = createBtn(MainShow::REMOVE_HIGH_LIGHTED, SLOT(SlotBtnRemoveHighLightedClicked()));
    layout->addWidget(removeBtn);

    auto addAnnoBtn = createBtn(PdfControl::ADD_NOTE, SLOT(SlotBtnAddAnnotationClicked()));
    layout->addWidget(addAnnoBtn);

    auto AddBookMarkBtn = createBtn(PdfControl::ADD_BK, SLOT(SlotBtnAddBookMarkClicked()));
    layout->addWidget(AddBookMarkBtn);
}
