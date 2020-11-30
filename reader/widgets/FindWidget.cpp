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
#include "BaseWidget.h"
#include "DocSheet.h"
#include "Utils.h"

#include <DDialogCloseButton>
#include <DIconButton>

#include <QHBoxLayout>
#include <QDesktopWidget>

FindWidget::FindWidget(DWidget *parent)
    : DFloatingWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    setMinimumSize(QSize(414, 60));

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
    this->move(nParentWidth - this->width() - 20, 20);

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
    m_pSearchEdit->clear();

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

void FindWidget::slotClearContent()
{
    QString strNewFind = m_pSearchEdit->text();

    if (strNewFind.isEmpty()) {
        setEditAlert(0);
    }
}

void FindWidget::slotEditAborted()
{
    m_strLastFindText.clear();

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
        bool bAlert = (iFlag == 1 ? true : false);

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
