/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "PagingWidget.h"

#include "frame/DocummentFileHelper.h"

PagingWidget::PagingWidget(CustomWidget *parent)
    : CustomWidget(QString("PagingWidget"), parent)
{
    resize(LEFTNORMALWIDTH, 56);

    m_pMsgList = { MSG_OPERATION_FIRST_PAGE, MSG_OPERATION_PREV_PAGE,
                   MSG_OPERATION_NEXT_PAGE, MSG_OPERATION_END_PAGE
                 };

    initWidget();
    initConnections();
    slotUpdateTheme();

    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

PagingWidget::~PagingWidget()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

/**
 * @brief PagingWidget::initWidget
 * 初始化界面
 */
void PagingWidget::initWidget()
{
    m_pTotalPagesLab = new CustomClickLabel(QString("/xxx"), this);
    m_pTotalPagesLab->setMinimumWidth(60);
    m_pTotalPagesLab->setFixedHeight(40);
    DFontSizeManager::instance()->bind(m_pTotalPagesLab, DFontSizeManager::T6);
    m_pTotalPagesLab->setForegroundRole(DPalette::Text);

    m_pPrePageBtn = new DIconButton(DStyle::SP_ArrowLeft);
    m_pPrePageBtn->setFixedSize(QSize(38, 38));
    connect(m_pPrePageBtn, SIGNAL(clicked()), SLOT(slotPrePage()));

    m_pNextPageBtn = new DIconButton(DStyle::SP_ArrowRight);
    m_pNextPageBtn->setFixedSize(QSize(38, 38));
    connect(m_pNextPageBtn, SIGNAL(clicked()), SLOT(slotNextPage()));

    m_pJumpPageSpinBox = new DSpinBox(this);
    m_pJumpPageSpinBox->setMinimum(1);
    m_pJumpPageSpinBox->setValue(1);
    m_pJumpPageSpinBox->setMinimumWidth(70);
    m_pJumpPageSpinBox->setFixedHeight(40);
    m_pJumpPageSpinBox->installEventFilter(this);
    m_pJumpPageSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    DFontSizeManager::instance()->bind(m_pJumpPageSpinBox, DFontSizeManager::T6);
    m_pJumpPageSpinBox->setForegroundRole(DPalette::Text);

    auto hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(7, 2, 7, 0);
    hLayout->addWidget(m_pJumpPageSpinBox);
    hLayout->addWidget(m_pTotalPagesLab);
    hLayout->addStretch(1);
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
            //过滤掉零开头的输入
            if (keyEvent->key() == Qt::Key_0 && m_pJumpPageSpinBox->text().isEmpty()) {
                return true;
            }

            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                int index = m_pJumpPageSpinBox->value() - 1;

//                if (m_preRow != index) {
//                    m_preRow = index;
                notifyMsg(MSG_DOC_JUMP_PAGE, QString::number(index));
//                }
            }
        } else if (event->type() == QEvent::KeyRelease &&
                   qobject_cast<DSpinBox *>(watched) == m_pJumpPageSpinBox) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_0) {
                QString strvalue = m_pJumpPageSpinBox->text();
                if (strvalue.startsWith("0")) {
                    strvalue = strvalue.right(strvalue.length() - 1);
                    if (strvalue.isEmpty()) {
                        // strvalue=QString("1");
                        m_pJumpPageSpinBox->clear();
                        return true;
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
    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)),
            SLOT(slotDealWithData(const int &, const QString &)));
//    connect(this, SIGNAL(sigDocFilePageChange(const QString &)),
//            SLOT(slotDealWithData(const int &, const QString &)));

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
    m_pTotalPagesLab->setText(QString("/%1").arg(pages));

    m_pJumpPageSpinBox->setMaximum(m_totalPage);

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
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return ConstantMsg::g_effective_res;
    }
    /*    if (msgType == MSG_FILE_PAGE_CHANGE) {
            emit sigDocFilePageChange(msgContent);
        } else*/
    if (msgType == MSG_OPERATION_UPDATE_THEME) { //  颜色主题切换
        emit sigUpdateTheme();
    }

    return 0;
}

//  跳转 指定页
//void PagingWidget::onJumpToSpecifiedPage(const int &nPage)
//{
//    //  跳转的页码 必须 大于0, 且 小于 总页码数
//    int nPageSize = DocummentFileHelper::instance()->getPageSNum();
//    if (nPage < 0 || nPage >= nPageSize) {
//        return;
//    }

//    m_preRow = nPage;

//    DocummentFileHelper::instance()->pageJump(nPage);
//}

void PagingWidget::slotUpdateTheme()
{
//    m_pTotalPagesLab->setThemePalette();
    if (m_pTotalPagesLab) {
        m_pTotalPagesLab->setForegroundRole(DPalette::Text);
    }
    if (m_pJumpPageSpinBox) {
        m_pJumpPageSpinBox->setForegroundRole(DPalette::Text);
    }
}

//void PagingWidget::SlotDocFilePageChange(const QString &msgContent)
//{
//    m_preRow = msgContent.toInt();
//}

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

//    m_preRow = inputData;

    m_pJumpPageSpinBox->setValue(currntPage);
}

void PagingWidget::slotDealWithData(const int &msgType, const QString &)
{
    switch (msgType) {
    case MSG_OPERATION_FIRST_PAGE:  //  第一页
        notifyMsg(MSG_DOC_JUMP_PAGE, "0");
        break;
    case MSG_OPERATION_PREV_PAGE:  //  上一页
        slotPrePage();
        break;
    case MSG_OPERATION_NEXT_PAGE:  //  下一页
        slotNextPage();
        break;
    case MSG_OPERATION_END_PAGE:  //  最后一页
        notifyMsg(MSG_DOC_JUMP_PAGE, QString::number(m_totalPage - FIRSTPAGES));
        break;
    default:
        break;
    }


}

//  按钮点击 上一页
void PagingWidget::slotPrePage()
{
    int nCurPage = DocummentFileHelper::instance()->currentPageNo();
    nCurPage--;
    notifyMsg(MSG_DOC_JUMP_PAGE, QString::number(nCurPage));
//    onJumpToSpecifiedPage(nCurPage);
}

//  按钮点击 下一页
void PagingWidget::slotNextPage()
{
    int nCurPage = DocummentFileHelper::instance()->currentPageNo();
    nCurPage++;
    notifyMsg(MSG_DOC_JUMP_PAGE, QString::number(nCurPage));
//    onJumpToSpecifiedPage(nCurPage);
}
