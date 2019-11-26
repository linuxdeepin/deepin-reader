#include "PagingWidget.h"

#include "frame/DocummentFileHelper.h"
#include "utils/utils.h"

#include <DFontSizeManager>

PagingWidget::PagingWidget(CustomWidget *parent) :
    CustomWidget(QString("PagingWidget"), parent)
{
    resize(250, 20);
    initWidget();

    initConnections();
    slotUpdateTheme();
}

/**
 * @brief PagingWidget::initWidget
 * 初始化界面
 */
void PagingWidget::initWidget()
{
    m_pTotalPagesLab = new CustomClickLabel(QString("/xxx") + tr("pages"), this);
    m_pTotalPagesLab->setFixedWidth(70);
    DFontSizeManager::instance()->bind(m_pTotalPagesLab, DFontSizeManager::T6);

    m_pPrePageBtn = new DIconButton(DStyle::SP_ArrowLeft);
    m_pPrePageBtn->setFixedSize(QSize(40, 40));
    connect(m_pPrePageBtn, SIGNAL(clicked()), SLOT(slotPrePage()));

    m_pNextPageBtn = new DIconButton(DStyle::SP_ArrowRight);
    m_pNextPageBtn->setFixedSize(QSize(40, 40));
    connect(m_pNextPageBtn, SIGNAL(clicked()), SLOT(slotNextPage()));

    m_pJumpPageSpinBox = new DSpinBox(this);
    m_pJumpPageSpinBox->setMinimum(1);
    m_pJumpPageSpinBox->setRange(1, 100);
    m_pJumpPageSpinBox->setValue(1);
    m_pJumpPageSpinBox->setFixedWidth(60);
    m_pJumpPageSpinBox->installEventFilter(this);
    m_pJumpPageSpinBox->setWrapping(true);
    m_pJumpPageSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    DFontSizeManager::instance()->bind(m_pJumpPageSpinBox, DFontSizeManager::T6);

    auto hLayout = new QHBoxLayout;
    hLayout->addStretch(1);
    hLayout->addWidget(m_pJumpPageSpinBox);
    hLayout->addWidget(m_pTotalPagesLab);
    hLayout->addWidget(m_pPrePageBtn);
    hLayout->addWidget(m_pNextPageBtn);
    hLayout->addStretch(1);
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
            //过滤掉零开头的输入
            if (keyEvent->key() == Qt::Key_0 && m_pJumpPageSpinBox->text().isEmpty()) {
                return  true;
            }

            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                int index = m_pJumpPageSpinBox->value() - 1;

                if (m_preRow != index) {
                    m_preRow = index;
                    DocummentFileHelper::instance()->pageJump(index);
                }
            }
        } else if (event->type() == QEvent::KeyRelease && qobject_cast<DSpinBox *>(watched) == m_pJumpPageSpinBox) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_0) {
                QString strvalue = m_pJumpPageSpinBox->text();
                if (strvalue.startsWith("0")) {
                    strvalue = strvalue.right(strvalue.length() - 1);
                    if (strvalue.isEmpty()) {
                        // strvalue=QString("1");
                        m_pJumpPageSpinBox->clear();
                        return  true;

                    }
                    m_pJumpPageSpinBox->setValue(strvalue.toInt());
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
    connect(this, SIGNAL(sigJudgeInputPage(const QString &)), this, SLOT(slotJudgeInputPage(const QString &)));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
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
    m_pTotalPagesLab->setText(QString("/%1").arg(pages) + tr("pages"));

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
int PagingWidget::dealWithData(const int &msgType, const QString &)
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
    case MSG_OPERATION_UPDATE_THEME:            //  颜色主题切换
        emit sigUpdateTheme();
        break;
    default:
        break;
    }
    return 0;
}

//  上一页
void PagingWidget::slotPrePage()
{
    int nCurPage = DocummentFileHelper::instance()->currentPageNo();
    nCurPage--;
    slotJumpToSpecifiedPage(nCurPage);
}

//  下一页
void PagingWidget::slotNextPage()
{
    int nCurPage = DocummentFileHelper::instance()->currentPageNo();
    nCurPage++;
    slotJumpToSpecifiedPage(nCurPage);
}

//  跳转 指定页
void PagingWidget::slotJumpToSpecifiedPage(const int &nPage)
{
    //  跳转的页码 必须 大于0, 且 小于 总页码数
    int nPageSize = DocummentFileHelper::instance()->getPageSNum();
    if (nPage < 0 || nPage >= nPageSize) {
        return;
    }

    m_preRow = nPage;

    DocummentFileHelper::instance()->pageJump(nPage);
}

void PagingWidget::slotJudgeInputPage(const QString &)
{
    QString t_strPage = QString::number(m_pJumpPageSpinBox->value());
    if (t_strPage.length() == 1 && m_pJumpPageSpinBox->value() == 0) {
        m_pJumpPageSpinBox->setValue(m_currntPage + 1);
    }
}

void PagingWidget::slotUpdateTheme()
{
    m_pTotalPagesLab->setThemePalette();
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

    m_preRow = inputData;

    m_pJumpPageSpinBox->setValue(currntPage);
}
