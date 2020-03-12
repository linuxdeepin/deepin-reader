#include "LeftSidebarWidget.h"

#include <DStackedWidget>
#include <QButtonGroup>
#include <QVBoxLayout>

#include "MainOperationWidget.h"

#include "docview/docummentproxy.h"
#include "pdfControl/DataStackedWidget.h"

#include "widgets/main/MainTabWidgetEx.h"

LeftSidebarWidget::LeftSidebarWidget(DWidget *parent)
    : CustomWidget(LEFT_SLIDERBAR_WIDGET, parent)
{
    setMinimumWidth(LEFTMINWIDTH);
    setMaximumWidth(LEFTMAXWIDTH);

    resize(LEFTNORMALWIDTH, this->height());

    m_pMsgList = {MSG_WIDGET_THUMBNAILS_VIEW};
    initWidget();

    onSetWidgetVisible(0);  //  默认 隐藏
    slotUpdateTheme();

    dApp->m_pModelService->addObserver(this);
}

LeftSidebarWidget::~LeftSidebarWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

void LeftSidebarWidget::SlotOpenFileOk(const QString &sPath)
{
    m_strBindPath = sPath;

    FileDataModel fdm = MainTabWidgetEx::Instance()->qGetFileData(sPath);
    int nShow = fdm.qGetData(Thumbnail);
    bool showLeft = nShow == 1 ? true : false;
    onSetWidgetVisible(showLeft);
}

void LeftSidebarWidget::onSetWidgetVisible(const int &nVis)
{
    this->setVisible(nVis);
}

void LeftSidebarWidget::slotUpdateTheme()
{
    updateWidgetTheme();
}

void LeftSidebarWidget::initWidget()
{
    auto pVBoxLayout = new QVBoxLayout;
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->setSpacing(0);
    this->setLayout(pVBoxLayout);

    m_pStackedWidget = new DataStackedWidget(this);
    connect(this, SIGNAL(sigAnntationMsg(const int &, const QString &)), m_pStackedWidget, SIGNAL(sigAnntationMsg(const int &, const QString &)));
    connect(m_pStackedWidget, SIGNAL(sigDeleteAnntation(const int &, const QString &)), this, SIGNAL(sigDeleteAnntation(const int &, const QString &)));

    pVBoxLayout->addWidget(m_pStackedWidget);

    m_pMainOperationWidget = new MainOperationWidget;
    connect(m_pMainOperationWidget, SIGNAL(sigShowStackWidget(const int &)), m_pStackedWidget, SLOT(slotSetStackCurIndex(const int &)));

    pVBoxLayout->addWidget(m_pMainOperationWidget, 0, Qt::AlignBottom);
}

int LeftSidebarWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {
        slotUpdateTheme();
    }

    int nRes = MSG_NO_OK;
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        SlotOpenFileOk(msgContent);
        m_pStackedWidget->dealWithData(msgType, msgContent);
        m_pMainOperationWidget->dealWithData(msgType, msgContent);
    } else {
        if (msgType == MSG_WIDGET_THUMBNAILS_VIEW) {
            onSetWidgetVisible(msgContent.toInt());
        }

        if (m_pMsgList.contains(msgType)) {
            nRes = MSG_OK;
        }

        if (nRes != MSG_OK) {
            nRes = m_pStackedWidget->dealWithData(msgType, msgContent);
        }
    }

    return nRes;
}

void LeftSidebarWidget::SetFindOperation(const int &iType)
{
    m_pMainOperationWidget->SetFindOperation(iType);
    m_pStackedWidget->SetFindOperation(iType);

    if (iType == E_FIND_CONTENT) {
        m_nSearch = 1;
        m_bOldVisible = this->isVisible();
        if (!m_bOldVisible) {
            this->setVisible(true);
        }
    } else if (iType == E_FIND_EXIT) {
        if (m_nSearch == 1) {
            m_nSearch = -1;
            this->setVisible(m_bOldVisible);
        } else {
            SlotOpenFileOk(m_strBindPath);
        }
    }
}
