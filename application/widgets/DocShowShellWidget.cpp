#include "DocShowShellWidget.h"

#include <DDialogCloseButton>
#include <QDesktopWidget>
#include <QStackedLayout>

#include "FileViewWidget.h"
#include "FileAttrWidget.h"
#include "SpinnerWidget.h"

#include "docview/docummentproxy.h"
#include "utils/PublicFunction.h"

#include "pdfControl/note/NoteViewWidget.h"
#include "widgets/main/MainTabWidgetEx.h"

DocShowShellWidget::DocShowShellWidget(CustomWidget *parent)
    : CustomWidget(DOC_SHOW_SHELL_WIDGET, parent)
{
    initWidget();

    dApp->m_pModelService->addObserver(this);
}

DocShowShellWidget::~DocShowShellWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

//  注释窗口
void DocShowShellWidget::onOpenNoteWidget(const QString &msgContent)
{
//    m_pFileViewWidget->onOpenNoteWidget(msgContent);
}

//  显示 当前 注释
void DocShowShellWidget::onShowNoteWidget(const QString &msgContent)
{
//    m_pFileViewWidget->onShowNoteWidget(msgContent);
}

void DocShowShellWidget::__ShowPageNoteWidget(const QString &msgContent)
{
//    m_pFileViewWidget->__ShowPageNoteWidget(msgContent);
}

void DocShowShellWidget::SlotOpenFileOk()
{
    m_playout->removeWidget(m_pSpiner);

    delete  m_pSpiner;
    m_pSpiner = nullptr;

    emit sigOpenFileOk();
}

void DocShowShellWidget::SlotFindOperation(const int &iType, const QString &strFind)
{
    emit sigFindOperation(iType);

    if (m_pFileViewWidget) {
//        m_pFileViewWidget->SetFindOperation(iType, strFind);
    }
}

int DocShowShellWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    return MSG_NO_OK;
}

bool DocShowShellWidget::OpenFilePath(const QString &sPath)
{
    m_strBindPath = sPath;
    return m_pFileViewWidget->OpenFilePath(sPath);
}

void DocShowShellWidget::ShowFindWidget()
{
    m_pFileViewWidget->ShowFindWidget();
}

void DocShowShellWidget::setFileChange(bool bchange)
{
    if (nullptr != m_pFileViewWidget)
        m_pFileViewWidget->setFileChange(bchange);
}

bool DocShowShellWidget::getFileChange()
{
    bool bchange = false;
    if (nullptr != m_pFileViewWidget)
        bchange = m_pFileViewWidget->getFileChange();
    return  bchange;
}

void DocShowShellWidget::initWidget()
{
    m_playout = new QStackedLayout;
    m_playout->setContentsMargins(0, 0, 0, 0);
    m_playout->setSpacing(0);

    m_pFileViewWidget = new FileViewWidget(this);
    connect(m_pFileViewWidget, SIGNAL(sigFileOpenOK()), SLOT(SlotOpenFileOk()));

    m_pSpiner = new SpinnerWidget(this);
    m_pSpiner->setSpinnerSize(QSize(36, 36));
    m_pSpiner->startSpinner();
    m_playout->addWidget(m_pSpiner);

    m_playout->addWidget(m_pFileViewWidget);

    this->setLayout(m_playout);
}
