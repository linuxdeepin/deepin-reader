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
    , m_parentWidget(parent)
{
    setMinimumSize(QSize(414, 60));

    setBlurBackgroundEnabled(true);

    initWidget();

    m_parentWidget->installEventFilter(this);
    this->setVisible(false);
}

FindWidget::~FindWidget()
{
}

void FindWidget::setDocSheet(DocSheet *sheet)
{
    m_docSheet = sheet;
}

void FindWidget::updatePosition()
{
    this->move(m_parentWidget->width() - this->width() - 10, m_yOff + 10);

    this->show();

    this->raise();
}

void FindWidget::setSearchEditFocus()
{
    m_pSearchEdit->lineEdit()->setFocus();
}

void FindWidget::onSearchStop()
{
    m_lastSearchText.clear();

    m_findPrevButton->setDisabled(true);
    m_findNextButton->setDisabled(true);

    setEditAlert(0);

    if (m_docSheet)
        m_docSheet->stopSearch();
}

void FindWidget::onSearchStart()
{
    QString searchText = m_pSearchEdit->text().trimmed();

    if ((searchText != "") && (m_lastSearchText != searchText)) {
        m_lastSearchText = searchText;
        setEditAlert(0);
        m_docSheet->startSearch(searchText);
        m_findPrevButton->setDisabled(false);
        m_findNextButton->setDisabled(false);
    }
}

void FindWidget::slotFindNextBtnClicked()
{
    if (m_docSheet)
        m_docSheet->jumpToNextSearchResult();
}

void FindWidget::slotFindPrevBtnClicked()
{
    if (m_docSheet)
        m_docSheet->jumpToPrevSearchResult();
}

void FindWidget::onTextChanged()
{
    if (m_pSearchEdit->text().isEmpty()) {
        setEditAlert(0);
    }
}

void FindWidget::initWidget()
{
    m_pSearchEdit = new DSearchEdit(this);
    m_pSearchEdit->setObjectName("findSearchEdit_P");
    m_pSearchEdit->lineEdit()->setObjectName("findSearchEdit");
    m_pSearchEdit->lineEdit()->setFocusPolicy(Qt::StrongFocus);
    m_pSearchEdit->setFixedSize(QSize(270, 36));
    connect(m_pSearchEdit, &DSearchEdit::returnPressed, this, &FindWidget::onSearchStart);
    connect(m_pSearchEdit, &DSearchEdit::textChanged, this, &FindWidget::onTextChanged);
    connect(m_pSearchEdit, &DSearchEdit::searchAborted, this, &FindWidget::onSearchStop);

    m_findPrevButton = new DIconButton(DStyle::SP_ArrowUp, this);
    m_findPrevButton->setObjectName("SP_ArrowUpBtn");
    m_findPrevButton->setToolTip(tr("Previous"));
    m_findPrevButton->setFixedSize(QSize(36, 36));
    m_findPrevButton->setIconSize(QSize(12, 12));
    m_findPrevButton->setDisabled(true);
    connect(m_findPrevButton, &DIconButton::clicked, this, &FindWidget::slotFindPrevBtnClicked);

    m_findNextButton = new DIconButton(DStyle::SP_ArrowDown, this);
    m_findNextButton->setObjectName("SP_ArrowDownBtn");
    m_findNextButton->setToolTip(tr("Next"));
    m_findNextButton->setFixedSize(QSize(36, 36));
    m_findNextButton->setIconSize(QSize(12, 12));
    m_findNextButton->setDisabled(true);
    connect(m_findNextButton, &DIconButton::clicked, this, &FindWidget::slotFindNextBtnClicked);

    DDialogCloseButton *closeButton = new DDialogCloseButton(this);
    closeButton->setObjectName("closeButton");
    closeButton->setIconSize(QSize(28, 28));
    closeButton->setFixedSize(QSize(30, 30));
    connect(closeButton, &DDialogCloseButton::clicked, this, &FindWidget::onCloseBtnClicked);

    auto layout = new QHBoxLayout;
    layout->setContentsMargins(8, 0, 6, 0);
    layout->addWidget(m_pSearchEdit);
    layout->addWidget(m_findPrevButton);
    layout->addWidget(m_findNextButton);
    layout->addWidget(closeButton);
    this->setLayout(layout);

    Utils::setObjectNoFocusPolicy(this);
}

void FindWidget::setEditAlert(const int &iFlag)
{
    if (m_pSearchEdit) {
        bool bAlert = (iFlag == 1 ? true : false);

        if (m_pSearchEdit->text().isEmpty())
            bAlert = false;

        m_pSearchEdit->setAlert(bAlert);
    }
}

void FindWidget::setYOff(int yOff)
{
    m_yOff = yOff;
}

void FindWidget::onCloseBtnClicked()
{
    onSearchStop();

    m_pSearchEdit->clear();

    this->hide();

    this->deleteLater();
}

void FindWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
        //这里不过滤掉的话,事件会跑到Browser上
        return;
    }

    DFloatingWidget::keyPressEvent(event);
}

bool FindWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == m_parentWidget && event->type() == QEvent::Resize) {
        updatePosition();
    }
    return DFloatingWidget::eventFilter(watched, event);
}
