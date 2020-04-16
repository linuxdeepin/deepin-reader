#include "TextOperationMenu.h"

#include "ColorWidgetAction.h"
#include "business/AppInfo.h"
#include "MsgHeader.h"
#include "utils/utils.h"
#include "DocSheet.h"

TextOperationMenu::TextOperationMenu(DWidget *parent)
    : CustomMenu(parent)
{
    initActions();
}

void TextOperationMenu::execMenu(DocSheet *sheet, const QPoint &showPoint, const bool &bHigh, const QString &sSelectText, const QString &sUuid)
{
    m_sheet = sheet;

    if (m_sheet.isNull())
        return;

    QList<int> pageList = dApp->m_pDBService->getBookMarkList(m_sheet->filePath());

    bool bBookState = pageList.contains(m_nClickPage);

    if (bBookState) {
        m_pAddBookMark->setProperty("data", 0);
        m_pAddBookMark->setText(tr("Remove bookmark"));
    } else {
        m_pAddBookMark->setProperty("data", 1);
        m_pAddBookMark->setText(tr("Add bookmark"));
    }

    m_strSelectText = sSelectText;

    if (m_nType == NOTE_ICON) {
        m_pRemoveHighLight->setText(tr("Remove annotation"));
        disconnect(m_pColorWidgetAction, SIGNAL(sigBtnGroupClicked(const int &)), this, SLOT(slotSetHighLight(const int &)));
        removeAction(m_pColorWidgetAction);
    } else if (m_nType == NOTE_HIGHLIGHT) {
        m_pRemoveHighLight->setText(tr("Remove highlight"));
        insertAction(m_pRemoveHighLight, m_pColorWidgetAction);
        connect(m_pColorWidgetAction, SIGNAL(sigBtnGroupClicked(const int &)), this, SLOT(slotSetHighLight(const int &)));
        if (m_pRemoveHighLight) {
            m_pRemoveHighLight->setEnabled(bHigh);
        }
        //  m_pColorWidgetAction->setVisible(true);
    }

    m_strNoteUuid = sUuid;
    m_pColorWidgetAction->setBtnAddLightState(bHigh);

    this->exec(showPoint);
}

void TextOperationMenu::setClickPoint(const QPoint &clickPoint)
{
    m_pClickPoint = clickPoint;
}

void TextOperationMenu::setClickPage(int nClickPage)
{
    m_nClickPage = nClickPage;
}

void TextOperationMenu::initActions()
{
    m_pCopy = createAction(tr("Copy"), SLOT(slotCopyClicked()));

    m_pColorWidgetAction = new ColorWidgetAction(this);
    connect(m_pColorWidgetAction, SIGNAL(sigBtnGroupClicked(const int &)), this, SLOT(slotSetHighLight(const int &)));
    this->addAction(m_pColorWidgetAction);
    this->addSeparator();

    m_pRemoveHighLight = createAction(tr("Remove highlight"), SLOT(slotRemoveHighLightClicked()));
    m_pAddNote = createAction(tr("Add note"), SLOT(slotAddNoteClicked()));
    m_pAddBookMark = createAction(tr("Add bookmark"), SLOT(slotAddBookMarkClicked()));

}

QAction *TextOperationMenu::createAction(const QString &text, const char *member)
{
    auto action = new  QAction(text, this);
    connect(action, SIGNAL(triggered()), member);
    this->addAction(action);
    this->addSeparator();
    return action;
}

void TextOperationMenu::slotSetHighLight(const int &nColor)
{
    m_pLightColor = nColor;

    bool bEnable = m_pRemoveHighLight->isEnabled();
    if (bEnable) {  //  移除高亮可以点,说明已有高亮, 点击操作 是 更新高亮
        QString sContent = QString::number(m_pLightColor) + Constant::sQStringSep +
                           m_strNoteUuid + Constant::sQStringSep +
                           QString::number(m_nClickPage);
        emit sigActionTrigger(MSG_NOTE_UPDATE_HIGHLIGHT_COLOR, sContent);
    } else {    //  移除高亮不可点,说明没有高亮, 点击操作 是 添加高亮
        int nSx = m_pStartPoint.x();
        int nSy = m_pStartPoint.y();

        int nEx = m_pEndPoint.x();
        int nEy = m_pEndPoint.y();

        QString sContent = QString::number(nSx) + Constant::sQStringSep +
                           QString::number(nSy) + Constant::sQStringSep +
                           QString::number(nEx) + Constant::sQStringSep +
                           QString::number(nEy) + Constant::sQStringSep +
                           QString::number(m_pLightColor);

        emit sigActionTrigger(MSG_NOTE_ADD_HIGHLIGHT_COLOR, sContent);
    }
    this->hide();
}

void TextOperationMenu::slotCopyClicked()
{
    Utils::copyText(m_strSelectText);
}

void TextOperationMenu::slotRemoveHighLightClicked()
{
    QString sContent = QString::number(m_pClickPoint.x()) + Constant::sQStringSep +  QString::number(m_pClickPoint.y());
    emit sigActionTrigger(MSG_NOTE_REMOVE_HIGHLIGHT, sContent);
}

void TextOperationMenu::slotAddNoteClicked()
{
    if (m_strNoteUuid == "") {
        QString msgContent = QString("%1").arg(m_nClickPage) + Constant::sQStringSep + QString("%1").arg(m_pClickPoint.x()) + Constant::sQStringSep + QString("%1").arg(m_pClickPoint.y());
        emit sigActionTrigger(MSG_OPERATION_TEXT_ADD_ANNOTATION, msgContent);
    } else {
        QString msgContent = m_strNoteUuid.trimmed() + Constant::sQStringSep + QString::number(m_nClickPage);
        emit sigActionTrigger(MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, msgContent);
    }
}

void TextOperationMenu::slotAddBookMarkClicked()
{

    int nData = m_pAddBookMark->property("data").toInt();
    if (nData == 0) {
        m_sheet->setBookMark(m_nClickPage, false);
    } else {
        m_sheet->setBookMark(m_nClickPage, true);
    }
}

void TextOperationMenu::setPEndPoint(const QPoint &pEndPoint)
{
    m_pEndPoint = pEndPoint;
}

void TextOperationMenu::setRemoveEnabled(bool enable)
{
    m_pRemoveHighLight->setEnabled(enable);
}

void TextOperationMenu::setPStartPoint(const QPoint &pStartPoint)
{
    m_pStartPoint = pStartPoint;
}
