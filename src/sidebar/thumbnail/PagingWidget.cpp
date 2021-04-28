/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     duanxiaohui<duanxiaohui@uniontech.com>
 *
 * Maintainer: duanxiaohui<duanxiaohui@uniontech.com>
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
#include "DocSheet.h"

#include <QValidator>

PagingWidget::PagingWidget(DocSheet *sheet, DWidget *parent)
    : CustomWidget(parent), m_sheet(sheet)
{
    initWidget();

    slotUpdateTheme();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &PagingWidget::slotUpdateTheme);
}

PagingWidget::~PagingWidget()
{

}

/**
 * @brief PagingWidget::initWidget
 * 初始化界面
 */
void PagingWidget::initWidget()
{
    m_pTotalPagesLab = new DLabel(QString("/xxx"));
    m_pTotalPagesLab->setAccessibleName("Label_TotalPage");
    QFont font = m_pTotalPagesLab->font();
    font.setPixelSize(14);

    m_pTotalPagesLab->setFont(font);
    m_pTotalPagesLab->setForegroundRole(DPalette::Text);

    int tW = 36;
    int tH = 36;

    m_pJumpPageLineEdit = new DLineEdit();
    m_pJumpPageLineEdit->setAccessibleName("Page");
    m_pJumpPageLineEdit->setObjectName("Edit_Page_P");
    m_pJumpPageLineEdit->lineEdit()->setObjectName("Edit_Page");
    m_pJumpPageLineEdit->lineEdit()->setAccessibleName("pageEdit");
    tW = 60;
    tH = 36;

    m_pJumpPageLineEdit->setFixedSize(tW, tH);
    m_pJumpPageLineEdit->setFocusPolicy(Qt::StrongFocus);
    connect(m_pJumpPageLineEdit, SIGNAL(returnPressed()), SLOT(SlotJumpPageLineEditReturnPressed()));
    connect(m_pJumpPageLineEdit, SIGNAL(editingFinished()), SLOT(onEditFinished()));
    m_pJumpPageLineEdit->setClearButtonEnabled(false);
    font = m_pJumpPageLineEdit->font();
    font.setPixelSize(14);

    m_pJumpPageLineEdit->setFont(font);
    m_pJumpPageLineEdit->setForegroundRole(DPalette::Text);

    m_pPrePageBtn = new DIconButton(DStyle::SP_ArrowLeft);
    m_pPrePageBtn->setAccessibleName("Button_ThumbnailPre");
    m_pPrePageBtn->setObjectName("thumbnailPreBtn");
    m_pPrePageBtn->setFixedSize(QSize(tW, tH));
    connect(m_pPrePageBtn, SIGNAL(clicked()), SLOT(slotPrePageBtnClicked()));

    m_pNextPageBtn = new DIconButton(DStyle::SP_ArrowRight);
    m_pNextPageBtn->setFixedSize(QSize(tW, tH));
    m_pNextPageBtn->setAccessibleName("Button_ThumbnailNext");
    m_pNextPageBtn->setObjectName("thumbnailNextBtn");
    connect(m_pNextPageBtn, SIGNAL(clicked()), SLOT(slotNextPageBtnClicked()));

    m_pCurrentPageLab = new DLabel("");
    m_pCurrentPageLab->setAccessibleName("CurrentPage");
    font = m_pCurrentPageLab->font();
    font.setPixelSize(14);

    m_pCurrentPageLab->setFont(font);
    m_pCurrentPageLab->setForegroundRole(DPalette::Text);

    auto hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(10, 6, 10, 6);
    hLayout->addWidget(m_pJumpPageLineEdit);
    hLayout->addSpacing(5);

    hLayout->addWidget(m_pCurrentPageLab);
    hLayout->addWidget(m_pTotalPagesLab);
    hLayout->addStretch(1);
    hLayout->addWidget(m_pPrePageBtn);
    hLayout->addWidget(m_pNextPageBtn);

    this->setLayout(hLayout);
}

void PagingWidget::slotUpdateTheme()
{
    if (m_pTotalPagesLab) {
        m_pTotalPagesLab->setForegroundRole(DPalette::Text);
    }

    if (m_pCurrentPageLab) {
        m_pCurrentPageLab->setForegroundRole(DPalette::Text);
    }
}

void PagingWidget::setBtnState(const int &currntPage, const int &totalPage)
{
    if (currntPage == 1) {                  //  第一页
        m_pPrePageBtn->setEnabled(false);
        if (totalPage == 1) {               //  也是最后一页
            m_pNextPageBtn->setEnabled(false);
        } else {
            m_pNextPageBtn->setEnabled(true);
        }
    } else if (currntPage == totalPage) {   //    最后一页
        m_pPrePageBtn->setEnabled(true);
        m_pNextPageBtn->setEnabled(false);
    } else {                                //  中间页
        m_pPrePageBtn->setEnabled(true);
        m_pNextPageBtn->setEnabled(true);
    }
}

void PagingWidget::setIndex(int index)
{
    if (nullptr == m_sheet)
        return;

    m_curIndex = index;
    int totalPage = m_sheet->pagesNumber();
    int inputData = index;
    int currntPage = inputData + 1;     //  + 1 是为了 数字 从1 开始显示
    setBtnState(currntPage, totalPage);

    if (m_pCurrentPageLab) {
        m_pCurrentPageLab->setText(QString::number(currntPage));
        QString sPage = m_sheet->getPageLabelByIndex(inputData);
        m_pJumpPageLineEdit->setText(sPage);
    } else {
        m_pJumpPageLineEdit->setText(QString::number(currntPage));
    }
}

/**
 * 文档打开成功
 * 1. 进行控件初始化
 * 2.设置总页数 和 当前页码
 *
 */
void PagingWidget::handleOpenSuccess()
{
    if (nullptr == m_sheet)
        return;

    bool isHasLabel = m_sheet->haslabel();
    if (!isHasLabel) {
        delete m_pCurrentPageLab;
        m_pCurrentPageLab =  nullptr;
    }

    int totalPage = m_sheet->pagesNumber();
    if (m_pCurrentPageLab == nullptr) {   //  不可读取页码, 则设置只能输入大于 0 的数字
        //  m_pJumpPageLineEdit->lineEdit()->setValidator(new QIntValidator(1, totalPage, this));
    }

    m_pTotalPagesLab->setText(QString("/ %1").arg(totalPage));
    int currentIndex = m_sheet->currentIndex();
    setIndex(currentIndex);
}

//  输入框  敲回车键 响应
void PagingWidget::SlotJumpPageLineEditReturnPressed()
{
    if (m_pCurrentPageLab == nullptr) {
        normalChangePage();
    } else {
        pageNumberJump();
    }
}

void PagingWidget::onEditFinished()
{
    if (m_curIndex >= 0)
        setIndex(m_curIndex);
}

void PagingWidget::normalChangePage()
{
    QString sText = m_pJumpPageLineEdit->text();
    int iPage = sText.toInt();
    if (iPage <= 0 || iPage > m_sheet->pagesNumber()) {
        m_sheet->showTips(tr("Invalid page number"), 1);
    } else {
        m_sheet->jumpToIndex(iPage - 1);
    }
}

void PagingWidget::pageNumberJump()
{
    int nPageSum = m_sheet->pagesNumber();
    QString sText = m_pJumpPageLineEdit->text();
    int iPage = m_sheet->getIndexByPageLable(sText);

    if (iPage > -1 && iPage < nPageSum) {   //  输入的页码 必须在 0-最大值 之间, 才可以
        m_sheet->jumpToIndex(iPage);
    } else {
        normalChangePage();
    }
}

void PagingWidget::slotPrePageBtnClicked()
{
    m_sheet->jumpToPrevPage();
}

void PagingWidget::slotNextPageBtnClicked()
{
    m_sheet->jumpToNextPage();
}

void PagingWidget::setTabOrderWidget(QList<QWidget *> &tabWidgetlst)
{
    tabWidgetlst << m_pJumpPageLineEdit;
    tabWidgetlst << m_pPrePageBtn;
    tabWidgetlst << m_pNextPageBtn;
}
