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

#include <QValidator>

#include "pdfControl/docview/docummentproxy.h"
#include "DocSheet.h"

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
    m_pTotalPagesLab = new CustomClickLabel(QString("/xxx"));
    QFont font = m_pTotalPagesLab->font();
    font.setPixelSize(14);
    m_pTotalPagesLab->setFont(font);
    m_pTotalPagesLab->setForegroundRole(DPalette::Text);

    int tW = 36;
    int tH = 36;

    m_pPrePageBtn = new DIconButton(DStyle::SP_ArrowLeft);
    m_pPrePageBtn->setFixedSize(QSize(tW, tH));
    connect(m_pPrePageBtn, SIGNAL(clicked()), SLOT(slotPrePageBtnClicked()));

    m_pNextPageBtn = new DIconButton(DStyle::SP_ArrowRight);
    m_pNextPageBtn->setFixedSize(QSize(tW, tH));
    connect(m_pNextPageBtn, SIGNAL(clicked()), SLOT(slotNextPageBtnClicked()));

    m_pJumpPageLineEdit = new DLineEdit();
    tW = 60;
    tH = 36;

    m_pJumpPageLineEdit->setFixedSize(tW, tH);
    connect(m_pJumpPageLineEdit, SIGNAL(returnPressed()), SLOT(SlotJumpPageLineEditReturnPressed()));
    connect(m_pJumpPageLineEdit, SIGNAL(editingFinished()), SLOT(onEditFinished()));
    m_pJumpPageLineEdit->setClearButtonEnabled(false);
    font = m_pJumpPageLineEdit->font();
    font.setPixelSize(14);
    m_pJumpPageLineEdit->setFont(font);
    m_pJumpPageLineEdit->setForegroundRole(DPalette::Text);

    m_pCurrantPageLab = new CustomClickLabel("");
    font = m_pCurrantPageLab->font();
    font.setPixelSize(14);
    m_pCurrantPageLab->setFont(font);
    m_pCurrantPageLab->setForegroundRole(DPalette::Text);

    auto hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(10, 6, 10, 6);
    hLayout->addWidget(m_pJumpPageLineEdit);
    hLayout->addSpacing(5);

    hLayout->addWidget(m_pCurrantPageLab);
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

    if (m_pCurrantPageLab) {
        m_pCurrantPageLab->setForegroundRole(DPalette::Text);
    }
}

void PagingWidget::__SetBtnState(const int &currntPage, const int &totalPage)
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
    __SetBtnState(currntPage, totalPage);

    if (m_pCurrantPageLab) {
        m_pCurrantPageLab->setText(QString::number(currntPage));
        QString sPage = m_sheet->pagenum2label(inputData);
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
        delete m_pCurrantPageLab;
        m_pCurrantPageLab =  nullptr;
    }

    int totalPage = m_sheet->pagesNumber();
    if (m_pCurrantPageLab == nullptr) {   //  不可读取页码, 则设置只能输入大于 0 的数字
        //  m_pJumpPageLineEdit->lineEdit()->setValidator(new QIntValidator(1, totalPage, this));
    }

    m_pTotalPagesLab->setText(QString("/ %1").arg(totalPage));
    int currentIndex = m_sheet->currentIndex();
    setIndex(currentIndex);
}

//  输入框  敲回车键 响应
void PagingWidget::SlotJumpPageLineEditReturnPressed()
{
    if (m_pCurrantPageLab == nullptr) {
        __NormalChangePage();
    } else {
        __PageNumberJump();
    }
}

void PagingWidget::onEditFinished()
{
    if (m_curIndex >= 0)
        setIndex(m_curIndex);
}

void PagingWidget::__NormalChangePage()
{
    QString sText = m_pJumpPageLineEdit->text();
    int iPage = sText.toInt();
    if (iPage <= 0 || iPage > m_sheet->pagesNumber()) {
        m_sheet->showTips(tr("Invalid page number"), 1);
    } else {
        m_sheet->jumpToIndex(iPage - 1);
        setFocus();
    }
}

void PagingWidget::__PageNumberJump()
{
    int nPageSum = m_sheet->pagesNumber();
    QString sText = m_pJumpPageLineEdit->text();
    int iPage = m_sheet->label2pagenum(sText);

    if (iPage > -1 && iPage < nPageSum) {   //  输入的页码 必须在 0-最大值 之间, 才可以
        m_sheet->jumpToIndex(iPage);
        setFocus();
    } else {
        m_sheet->showTips(tr("Invalid page number"), 1);
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
