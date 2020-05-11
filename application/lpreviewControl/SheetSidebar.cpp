#include "SheetSidebar.h"

#include <DStackedWidget>
#include <QButtonGroup>
#include <QVBoxLayout>

#include "widgets/MainOperationWidget.h"

#include "pdfControl/docview/docummentproxy.h"
#include "lpreviewControl/DataStackedWidget.h"
#include "DocSheet.h"

SheetSidebar::SheetSidebar(DocSheet *parent)
    : CustomWidget(parent), m_sheet(parent)
{
    int tW = LEFTMINWIDTH;
    int tH = LEFTMINWIDTH;
    dApp->adaptScreenView(tW, tH);
    setMinimumWidth(tW);
    tW = LEFTMAXWIDTH;
    dApp->adaptScreenView(tW, tH);
    setMaximumWidth(tW);
    tW = LEFTNORMALWIDTH;
    dApp->adaptScreenView(tW, tH);
    resize(tW, this->height());

    initWidget();
    onSetWidgetVisible(0);  //  默认 隐藏
    slotUpdateTheme();

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &SheetSidebar::slotUpdateTheme);
}

SheetSidebar::~SheetSidebar()
{

}

void SheetSidebar::setBookMark(int page, int state)
{
    m_pStackedWidget->handleBookMark(page, state);
}

void SheetSidebar::handleOpenSuccess()
{
    onSetWidgetVisible(m_sheet->getOper(Thumbnail).toInt());
    m_pStackedWidget->handleOpenSuccess();
    m_pMainOperationWidget->handleOpenSuccess();
}

void SheetSidebar::onSetWidgetVisible(const int &nVis)
{
    this->setVisible(nVis);
}

void SheetSidebar::slotUpdateTheme()
{
    updateWidgetTheme();
}

void SheetSidebar::initWidget()
{
    auto pVBoxLayout = new QVBoxLayout;
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->setSpacing(0);
    this->setLayout(pVBoxLayout);

    m_pStackedWidget = new DataStackedWidget(m_sheet, this);
    connect(m_pStackedWidget, SIGNAL(sigDeleteAnntation(const int &, const QString &)), this, SIGNAL(sigDeleteAnntation(const int &, const QString &)));
    pVBoxLayout->addWidget(m_pStackedWidget);

    m_pMainOperationWidget = new MainOperationWidget(m_sheet, this);
    connect(m_pMainOperationWidget, SIGNAL(sigShowStackWidget(const int &)), m_pStackedWidget, SLOT(slotSetStackCurIndex(const int &)));

    pVBoxLayout->addWidget(m_pMainOperationWidget, 0, Qt::AlignBottom);
}

void SheetSidebar::resizeEvent(QResizeEvent *event)
{
    int width = event->size().width();

    double scale = static_cast<double>(width) / static_cast<double>(LEFTMINWIDTH);

    m_pStackedWidget->adaptWindowSize(scale);

    CustomWidget::resizeEvent(event);
}

void SheetSidebar::handleFindOperation(int type)
{
    m_pMainOperationWidget->handleFindOperation(type);
    m_pStackedWidget->handleFindOperation(type);

    if (type == E_FIND_CONTENT) {
        m_nSearch = 1;
        m_bOldVisible = this->isVisible();
        if (!m_bOldVisible) {
            this->setVisible(true);
        }
    } else if (type == E_FIND_EXIT) {
        if (m_nSearch == 1) {
            m_nSearch = -1;
            this->setVisible(m_bOldVisible);
        }
    }
}

void SheetSidebar::handleFindContentComming(const stSearchRes &res)
{
    m_pStackedWidget->handleFindContentComming(res);
}

int SheetSidebar::handleFindFinished()
{
    return m_pStackedWidget->handleFindFinished();
}

void SheetSidebar::handleRotate(int rotate)
{
    m_pStackedWidget->handleRotate(rotate);
}

void SheetSidebar::handleUpdateThumbnail(const int &page)
{
    m_pStackedWidget->handleUpdateThumbnail(page);
}

void SheetSidebar::handleAnntationMsg(const int &msg, const QString &text)
{
    m_pStackedWidget->handleAnntationMsg(msg, text);
}

void SheetSidebar::onPageChanged(int page)
{
    m_pStackedWidget->handlePage(page);
}
