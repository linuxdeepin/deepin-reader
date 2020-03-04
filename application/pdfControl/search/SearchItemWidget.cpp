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
#include "SearchItemWidget.h"
#include <DApplication>
#include <DApplicationHelper>
#include <QClipboard>
#include <QTextLayout>

#include "utils/utils.h"

SearchItemWidget::SearchItemWidget(DWidget *parent)
    : CustomItemWidget(QString("SearchItemWidget"), parent)
{
    initWidget();
}

SearchItemWidget::~SearchItemWidget()
{
}

//  搜索显示内容
void SearchItemWidget::setTextEditText(const QString &contant)
{
    m_strNote = contant;
    if (m_pTextLab) {
        m_pTextLab->clear();
        QString note = m_strNote;

        note.replace(QChar('\n'), QString(""));
        note.replace(QChar('\t'), QString(""));
        m_pTextLab->setText(calcText(m_pTextLab->font(), note, m_pTextLab->size()));
    }
}

//  搜索結果
void SearchItemWidget::setSerchResultText(const QString &result)
{
    if (m_pSearchResultNum) {
        m_pSearchResultNum->setText(result);
    }
}

bool SearchItemWidget::bSelect()
{
    if (m_pPicture) {
        return m_pPicture->bSelect();
    }
    return false;
}

void SearchItemWidget::setBSelect(const bool &paint)
{
    if (m_pPicture) {
        m_pPicture->setSelect(paint);
    }
    m_bPaint = paint;
    update();
}

void SearchItemWidget::slotUpdateTheme()
{
    if (m_pPageNumber) {
        m_pPageNumber->setForegroundRole(DPalette::TextTitle);
    }
    if (m_pTextLab) {
        m_pTextLab->setForegroundRole(QPalette::BrightText);
    }
    if (m_pSearchResultNum) {
        m_pSearchResultNum->setForegroundRole(DPalette::TextTips);
    }
}

void SearchItemWidget::initWidget()
{
    auto t_vLayoutPicture = new QVBoxLayout;
    t_vLayoutPicture->setContentsMargins(0, 3, 0, 0);
    m_pPicture = new ImageLabel(this);
    m_pPicture->setFixedSize(QSize(48, 68));
    m_pPicture->setAlignment(Qt::AlignCenter);
    t_vLayoutPicture->addWidget(m_pPicture);
    t_vLayoutPicture->addStretch(1);

    m_pPageNumber = new PageNumberLabel(this);
    m_pPageNumber->setMinimumWidth(31);
    m_pPageNumber->setFixedHeight(18);
    m_pPageNumber->setForegroundRole(DPalette::WindowText);
    DFontSizeManager::instance()->bind(m_pPageNumber, DFontSizeManager::T8);

    m_pSearchResultNum = new DLabel(this);
    m_pSearchResultNum->setMinimumWidth(31);
    m_pSearchResultNum->setFixedHeight(18);
    m_pSearchResultNum->setForegroundRole(DPalette::TextTips);
    DFontSizeManager::instance()->bind(m_pSearchResultNum, DFontSizeManager::T10);

    m_pTextLab = new DLabel(this);
    m_pTextLab->setTextFormat(Qt::PlainText);
    m_pTextLab->setFixedHeight(54);
    m_pTextLab->setMinimumWidth(80);
    //    m_pTextLab->setMaximumWidth(349);
    m_pTextLab->setFrameStyle(QFrame::NoFrame);
    m_pTextLab->setWordWrap(true);
    m_pTextLab->setAlignment(Qt::AlignLeft);
    m_pTextLab->setAlignment(Qt::AlignTop);
    m_pTextLab->setForegroundRole(DPalette::BrightText);
    DFontSizeManager::instance()->bind(m_pTextLab, DFontSizeManager::T9);

    auto hLine = new DHorizontalLine(this);

    auto t_hLayout = new QHBoxLayout;
    t_hLayout->setContentsMargins(0, 0, 0, 0);
    t_hLayout->setSpacing(0);
    t_hLayout->addWidget(m_pPageNumber);
    t_hLayout->addWidget(m_pSearchResultNum);

    auto t_vLayout = new QVBoxLayout;
    t_vLayout->setContentsMargins(15, 0, 10, 0);
    t_vLayout->setSpacing(0);
    t_vLayout->addItem(t_hLayout);
    t_vLayout->addWidget(m_pTextLab);
    t_vLayout->addStretch(1);
    t_vLayout->addWidget(hLine);

    auto m_pHLayout = new QHBoxLayout;

    m_pHLayout->setSpacing(1);
    m_pHLayout->setContentsMargins(0, 0, 10, 0);
//    m_pHLayout->addWidget(m_pPicture);
    m_pHLayout->addItem(t_vLayoutPicture);
    m_pHLayout->addItem(t_vLayout);
    m_pHLayout->setSpacing(1);

    this->setLayout(m_pHLayout);
}

int SearchItemWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {
        slotUpdateTheme();
    }
    return MSG_NO_OK;
}

void SearchItemWidget::resizeEvent(QResizeEvent *event)
{
    QString note = m_strNote;

    note.replace(QChar('\n'), QString(""));
    note.replace(QChar('\t'), QString(""));
    m_pTextLab->setText(calcText(m_pTextLab->font(), note, m_pTextLab->size()));

    CustomItemWidget::resizeEvent(event);
}

void SearchItemWidget::paintEvent(QPaintEvent *e)
{
    CustomItemWidget::paintEvent(e);

    //  涉及到 主题颜色
    if (m_bPaint) {
        m_pPicture->setForegroundRole(DPalette::Highlight);
    } else {
        m_pPicture->setForegroundRole(QPalette::Shadow);
    }
}
