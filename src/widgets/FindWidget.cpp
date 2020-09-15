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
#include "FindWidget.h"
#include "widgets/CustomWidget.h"
#include "DocSheet.h"

#include <DDialogCloseButton>
#include <DIconButton>

#include <QDebug>
#include <QHBoxLayout>
#include <QDesktopWidget>

FindWidget::FindWidget(DWidget *parent)
    : DFloatingWidget(parent)
{
    int tW = 414;
    int tH = 60;

    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(QSize(tW, tH));
    setBlurBackgroundEnabled(true);

    initWidget();

    this->setVisible(false);
}

FindWidget::~FindWidget()
{
}

void FindWidget::setDocSheet(DocSheet *sheet)
{
    m_docSheet = sheet;
}

void FindWidget::showPosition(const int &nParentWidth)
{
    int nWidget = this->width();
    this->move(nParentWidth - nWidget - 20, 20);
    this->show();
    this->raise();
}

void FindWidget::setSearchEditFocus()
{
    m_pSearchEdit->lineEdit()->setFocus();
}

void FindWidget::stopSearch()
{
    findCancel();
}

void FindWidget::findCancel()
{
    m_pSearchEdit->setText("");
    slotEditAborted();
    m_strLastFindText = "";
    this->close();
}

void FindWidget::handleContentChanged()
{
    QString strFind = m_pSearchEdit->text().trimmed();
    if ((strFind != "") && (m_strLastFindText != strFind)) {
        m_strLastFindText = strFind;
        setEditAlert(0);
        m_docSheet->handleFindContent(strFind);
    }
}

void FindWidget::slotFindNextBtnClicked()
{
    if (m_docSheet)
        m_docSheet->handleFindNext();
}

void FindWidget::slotFindPrevBtnClicked()
{
    if (m_docSheet)
        m_docSheet->handleFindPrev();
}

//  文本内容变化, 为空, 则取消红色提示
void FindWidget::slotClearContent()
{
    QString strNewFind = m_pSearchEdit->text();
    if (strNewFind == "") {
        setEditAlert(0);
    }
}

//  点击 搜索框 里面 的 x
void FindWidget::slotEditAborted()
{
    m_strLastFindText = "";
    setEditAlert(0);
    if (m_docSheet)
        m_docSheet->handleFindExit();
}

void FindWidget::initWidget()
{
    m_pSearchEdit = new DSearchEdit(this);
    m_pSearchEdit->lineEdit()->setObjectName("findSearchEdit");
    m_pSearchEdit->lineEdit()->setFocusPolicy(Qt::StrongFocus);
    m_pSearchEdit->setFixedSize(QSize(270, 36));
    connect(m_pSearchEdit, &DSearchEdit::returnPressed, this, &FindWidget::handleContentChanged);
    connect(m_pSearchEdit, &DSearchEdit::textChanged, this, &FindWidget::slotClearContent);
    connect(m_pSearchEdit, &DSearchEdit::searchAborted, this, &FindWidget::slotEditAborted);

    DIconButton *findPrevButton = new DIconButton(DStyle::SP_ArrowUp, this);
    findPrevButton->setObjectName("SP_ArrowUpBtn");
    findPrevButton->setToolTip(tr("Previous"));
    findPrevButton->setFixedSize(QSize(36, 36));
    findPrevButton->setIconSize(QSize(12, 12));
    connect(findPrevButton, &DIconButton::clicked, this, &FindWidget::slotFindPrevBtnClicked);

    DIconButton *findNextButton = new DIconButton(DStyle::SP_ArrowDown, this);
    findNextButton->setObjectName("SP_ArrowDownBtn");
    findNextButton->setToolTip(tr("Next"));
    findNextButton->setFixedSize(QSize(36, 36));
    findNextButton->setIconSize(QSize(12, 12));
    connect(findNextButton, &DIconButton::clicked, this, &FindWidget::slotFindNextBtnClicked);

    DDialogCloseButton *closeButton = new DDialogCloseButton(this);
    closeButton->setObjectName("closeButton");
    closeButton->setIconSize(QSize(28, 28));
    closeButton->setFixedSize(QSize(30, 30));
    connect(closeButton, &DDialogCloseButton::clicked, this, &FindWidget::findCancel);

    auto layout = new QHBoxLayout;
    layout->setContentsMargins(8, 0, 6, 0);
    layout->addWidget(m_pSearchEdit);
    layout->addWidget(findPrevButton);
    layout->addWidget(findNextButton);
    layout->addWidget(closeButton);
    this->setLayout(layout);

    Utils::setObjectNoFocusPolicy(this);
}

//  设置 提醒红色
void FindWidget::setEditAlert(const int &iFlag)
{
    if (m_pSearchEdit) {
        bool bAlert = iFlag == 1 ? true : false;
        if (m_pSearchEdit->text().isEmpty())
            bAlert = false;
        m_pSearchEdit->setAlert(bAlert);
    }
}

void FindWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
        //这里不过滤掉的话,事件会跑到Browser上
        return;
    }
    DFloatingWidget::keyPressEvent(event);
}
