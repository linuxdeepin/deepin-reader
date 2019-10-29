#include "TextOperationMenu.h"
#include "ColorWidgetAction.h"
#include "subjectObserver/MsgHeader.h"
#include "controller/MsgSubject.h"
#include "docview/docummentproxy.h"
#include "controller/DataManager.h"

TextOperationMenu::TextOperationMenu(DWidget *parent)
    : DMenu (parent)
{
    initMenu();
}

void TextOperationMenu::execMenu(const QPoint &showPoint, const bool &bHigh, const QString &sSelectText, const QString &sUuid)
{
    bool bBookState = DataManager::instance()->bIsBookMarkState();
    m_pAddBookMark->setEnabled(!bBookState);

    m_strSelectText = sSelectText;
    if (m_strSelectText == "") {
        m_pCopy->setEnabled(false);
    } else {
        m_pCopy->setEnabled(true);
    }

    m_strNoteUuid = sUuid;
    m_pRemoveHighLight->setEnabled(bHigh);

    this->exec(showPoint);
}

void TextOperationMenu::initMenu()
{
    m_pCopy = createAction(tr("copy"), SLOT(slotCopyClicked()));

    createAction(tr("add high light"), SLOT(slotAddHighLightClicked()));

    auto colorAction = new ColorWidgetAction(this);
    connect(colorAction, SIGNAL(sigBtnGroupClicked(int)), this, SLOT(slotSetHighLight(int)));
    this->addAction(colorAction);

    m_pRemoveHighLight = createAction(tr("remove high light"), SLOT(slotRemoveHighLightClicked()));
    createAction(tr("add note"), SLOT(slotAddNoteClicked()));
    m_pAddBookMark = createAction(tr("add bookmark"), SLOT(slotAddBookMarkClicked()));
}

QAction *TextOperationMenu::createAction(const QString &text, const char *member)
{
    auto action = new  QAction(text, this);
    connect(action, SIGNAL(triggered()), member);
    this->addAction(action);

    return action;
}

void TextOperationMenu::sendMsgToFrame(const int &msgType, const QString &msgContent)
{
    MsgSubject::getInstance()->sendMsg(nullptr, msgType, msgContent);
}

void TextOperationMenu::slotSetHighLight(int nColor)
{
    m_pLightColor = nColor;
}

void TextOperationMenu::slotCopyClicked()
{
    sendMsgToFrame(MSG_OPERATION_TEXT_COPY, m_strSelectText);
}

void TextOperationMenu::slotAddHighLightClicked()
{
    sendMsgToFrame(MSG_OPERATION_TEXT_ADD_HIGHLIGHTED, QString("%1").arg(m_pLightColor));
}

void TextOperationMenu::slotRemoveHighLightClicked()
{
    sendMsgToFrame(MSG_OPERATION_TEXT_REMOVE_HIGHLIGHTED);
}

void TextOperationMenu::slotAddNoteClicked()
{
    sendMsgToFrame(MSG_OPERATION_TEXT_ADD_ANNOTATION,
                   QString("%1").arg(m_strNoteUuid));
}

void TextOperationMenu::slotAddBookMarkClicked()
{
    int nCurPage = DocummentProxy::instance()->currentPageNo();
    sendMsgToFrame(MSG_OPERATION_TEXT_ADD_BOOKMARK, QString("%1").arg(nCurPage));
}
