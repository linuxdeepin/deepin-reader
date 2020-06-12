/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
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
#include "TitleWidget.h"
#include "widgets/ScaleWidget.h"
#include "DocSheet.h"

#include <QHBoxLayout>

TitleWidget::TitleWidget(DWidget *parent)
    : CustomWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);
    initWidget();
}

TitleWidget::~TitleWidget()
{

}

void TitleWidget::initBtns()
{
    m_pThumbnailBtn = createBtn(tr("Thumbnails"), true);
    m_pThumbnailBtn->setIcon(QIcon::fromTheme(Pri::g_module + "thumbnails"));
    m_pThumbnailBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_pThumbnailBtn, SIGNAL(clicked()), SLOT(onThumbnailBtnClicked()));
}

void TitleWidget::initWidget()
{
    initBtns();

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(5, 0, 0, 0);
    hlayout->setSpacing(10);
    setLayout(hlayout);
    hlayout->addWidget(m_pThumbnailBtn);

    m_pSw = new ScaleWidget;
    m_pSw->setDisabled(true);
    hlayout->addWidget(m_pSw);
    hlayout->addStretch(1);
}

void TitleWidget::setBtnDisable(const bool &bAble)
{
    m_pThumbnailBtn->setDisabled(bAble);
    m_pSw->setDisabled(bAble);
}

void TitleWidget::onCurSheetChanged(DocSheet *sheet)
{
    m_curSheet = sheet;

    if (nullptr == m_curSheet || m_curSheet->type() == Dr::Unknown) {
        setBtnDisable(true);
        return;

    } else if (Dr::PDF == m_curSheet->type()) {
        setBtnDisable(false);
        m_pThumbnailBtn->setChecked(m_curSheet->operation().sidebarVisible);
        m_pSw->setSheet(m_curSheet);

    } else if (Dr::DjVu == m_curSheet->type()) {
        m_pThumbnailBtn->setDisabled(false);
        m_pSw->setDisabled(false);

        m_pThumbnailBtn->setChecked(m_curSheet->operation().sidebarVisible);
        m_pSw->setSheet(m_curSheet);
    }
}

void TitleWidget::onThumbnailBtnClicked()
{
    if (m_curSheet.isNull())
        return;

    bool rl = m_pThumbnailBtn->isChecked();
    m_curSheet->setSidebarVisible(rl);

}

void TitleWidget::onFindOperation(const int &sAction)
{
    if (sAction == E_FIND_CONTENT) {
        m_pThumbnailBtn->setChecked(true);
    } else if (sAction == E_FIND_EXIT) {
        if (m_curSheet) {
            bool close = m_curSheet->operation().sidebarVisible;
            m_pThumbnailBtn->setChecked(close);
        }
    }
}

DPushButton *TitleWidget::createBtn(const QString &btnName, bool bCheckable)
{
    int tW = 36;
    DPushButton *btn = new DPushButton(this);
    btn->setFixedSize(QSize(tW, tW));
    btn->setIconSize(QSize(tW, tW));
    btn->setToolTip(btnName);
    btn->setCheckable(bCheckable);
    btn->setDisabled(true);
    return btn;
}

void TitleWidget::setControlEnabled(const bool &enable)
{
    m_pThumbnailBtn->setChecked(false);
    m_pThumbnailBtn->setEnabled(enable);
}
