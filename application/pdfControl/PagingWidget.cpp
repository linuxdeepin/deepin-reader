#include "PagingWidget.h"
#include <QDebug>

#include "translator/PdfControl.h"

PagingWidget::PagingWidget(CustomWidget *parent) :
    CustomWidget(QString("PagingWidget"), parent)
{
    resize(250, 20);
    initWidget();

    initConnections();
}

/**
 * @brief PagingWidget::initWidget
 * 初始化界面
 */
void PagingWidget::initWidget()
{
    m_pTotalPagesLab = new DLabel(this);
    m_pTotalPagesLab->setText(QString("/xxx") + PdfControl::PAGES);
    m_pTotalPagesLab->setMinimumWidth(80);

    m_pPrePageBtn = new DIconButton(DStyle::SP_ArrowLeft);
    m_pPrePageBtn->setFixedSize(QSize(40, 40));
    connect(m_pPrePageBtn, SIGNAL(clicked()), SLOT(slotPrePage()));

    m_pNextPageBtn = new DIconButton(DStyle::SP_ArrowRight);
    m_pNextPageBtn->setFixedSize(QSize(40, 40));
    connect(m_pNextPageBtn, SIGNAL(clicked()), SLOT(slotNextPage()));

    m_pJumpPageSpinBox = new DSpinBox(this);
    m_pJumpPageSpinBox->setRange(1, 100);
    m_pJumpPageSpinBox->setValue(1);
    m_pJumpPageSpinBox->setMinimumWidth(50);
    m_pJumpPageSpinBox->installEventFilter(this);
    m_pJumpPageSpinBox->setWrapping(true);
    m_pJumpPageSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_pJumpPageSpinBox);
    hLayout->addWidget(m_pTotalPagesLab);
    hLayout->addWidget(m_pPrePageBtn);
    hLayout->addWidget(m_pNextPageBtn);
    this->setLayout(hLayout);
}

/**
 * @brief PagingWidget::eventFilter
 * 输入框响应回车事件
 * @param watched
 * @param event
 * @return
 */
bool PagingWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_pJumpPageSpinBox) {
        if (event->type() == QEvent::KeyPress) {

            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {

                int index = m_pJumpPageSpinBox->value() - 1;
                if (m_preRow != index) {
                    m_preRow = index;
                    DocummentProxy::instance()->pageJump(index);
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void PagingWidget::initConnections()
{
    connect(this, SIGNAL(sigJumpToPrevPage()), this, SLOT(slotPrePage()));
    connect(this, SIGNAL(sigJumpToNextPage()), this, SLOT(slotNextPage()));
    connect(this, SIGNAL(sigJumpToSpecifiedPage(const int &)), this, SLOT(slotJumpToSpecifiedPage(const int &)));
}

/**
 * @brief PagingWidget::setTotalPages
 * 设置 总页数
 * @param pages
 */
void PagingWidget::setTotalPages(int pages)
{
    m_totalPage = pages;
    m_currntPage = FIRSTPAGES;
    m_pTotalPagesLab->setText(QString("/%1").arg(pages) + PdfControl::PAGES);

    m_pJumpPageSpinBox->setRange(1, m_totalPage);

    m_pPrePageBtn->setEnabled(false);

    if (m_totalPage == 1) {
        m_pNextPageBtn->setEnabled(false);
    }
}

/**
 * @brief PagingWidget::dealWithData
 * 处理全局消息接口
 * @param msgType
 * @return
 */
int PagingWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPERATION_FIRST_PAGE:              //  第一页
        emit sigJumpToSpecifiedPage(0);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_PREV_PAGE:               //  上一页
        emit sigJumpToPrevPage();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_NEXT_PAGE:               //  下一页
        emit sigJumpToNextPage();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_END_PAGE:                //  最后一页
        emit sigJumpToSpecifiedPage(m_totalPage - FIRSTPAGES);
        return ConstantMsg::g_effective_res;
    case MSG_NOTIFY_KEY_MSG: {
        if (msgContent == "Up") {
            emit sigJumpToPrevPage();
            return ConstantMsg::g_effective_res;
        } else if (msgContent == "Down") {
            emit sigJumpToNextPage();
            return ConstantMsg::g_effective_res;
        }
    }
    break;
    }
    return 0;
}

//  上一页
void PagingWidget::slotPrePage()
{
    int nCurPage = DocummentProxy::instance()->currentPageNo();
    nCurPage--;
    slotJumpToSpecifiedPage(nCurPage);
}

//  下一页
void PagingWidget::slotNextPage()
{
    int nCurPage = DocummentProxy::instance()->currentPageNo();
    nCurPage++;
    slotJumpToSpecifiedPage(nCurPage);
}

//  跳转 指定页
void PagingWidget::slotJumpToSpecifiedPage(const int &nPage)
{
    //  跳转的页码 必须 大于0, 且 小于 总页码数
    int nPageSize = DocummentProxy::instance()->getPageSNum();
    if (nPage < 0 || nPage == nPageSize) {
        return;
    }

    DocummentProxy::instance()->pageJump(nPage);
}

//  设置当前页码, 进行比对,是否可以 上一页\下一页
void PagingWidget::setCurrentPageValue(const int &inputData)
{
    int currntPage = inputData + 1;
    if (currntPage == FIRSTPAGES) {
        m_pPrePageBtn->setEnabled(false);
        m_pNextPageBtn->setEnabled(true);
    } else if (currntPage == m_totalPage) {
        m_pPrePageBtn->setEnabled(true);
        m_pNextPageBtn->setEnabled(false);
    } else {
        m_pPrePageBtn->setEnabled(true);
        m_pNextPageBtn->setEnabled(true);
    }

    m_pJumpPageSpinBox->setValue(currntPage);
}
