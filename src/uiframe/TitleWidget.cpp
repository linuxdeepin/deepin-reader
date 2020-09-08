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
#include "ScaleWidget.h"
#include "DocSheet.h"
#include "MsgHeader.h"
#include "MainWindow.h"

#include <QHBoxLayout>

TitleWidget::TitleWidget(DWidget *parent)
    : CustomWidget(parent)
{
    initWidget();
}

TitleWidget::~TitleWidget()
{

}

void TitleWidget::initBtns()
{
    m_pThumbnailBtn = createBtn(tr("Thumbnails"), true);
    m_pThumbnailBtn->setObjectName("Thumbnails");
    m_pThumbnailBtn->setIcon(QIcon::fromTheme(QString("dr_") + "thumbnails"));
    connect(m_pThumbnailBtn, SIGNAL(clicked(bool)), SLOT(onThumbnailBtnClicked(bool)));
}

void TitleWidget::initWidget()
{
    initBtns();

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(6, 0, 6, 0);
    hlayout->setSpacing(10);
    setLayout(hlayout);
    hlayout->addWidget(m_pThumbnailBtn);

    m_pSw = new ScaleWidget;
    m_pSw->setDisabled(true);
    hlayout->addWidget(m_pSw);
    hlayout->addStretch(1);

    DIconButton *decBtn = m_pSw->findChild<DIconButton *>("SP_DecreaseElement");
    DIconButton *incBtn = m_pSw->findChild<DIconButton *>("SP_IncreaseElement");
    DLineEdit *scaleEdit = m_pSw->findChild<DLineEdit *>("scaleEdit");
    this->setTabOrder(m_pThumbnailBtn, decBtn);
    this->setTabOrder(decBtn, scaleEdit);
    this->setTabOrder(scaleEdit, incBtn);

    DIconButton *optBtn = parent()->findChild<DIconButton *>("DTitlebarDWindowOptionButton");
    DIconButton *fullBtn = parent()->findChild<DIconButton *>("DTitlebarDWindowQuitFullscreenButton");
    DIconButton *minBtn = parent()->findChild<DIconButton *>("DTitlebarDWindowMinButton");
    DIconButton *maxBtn = parent()->findChild<DIconButton *>("DTitlebarDWindowMaxButton");
    DIconButton *closeBtn = parent()->findChild<DIconButton *>("DTitlebarDWindowCloseButton");

    this->setTabOrder(incBtn, optBtn);
    if (nullptr != fullBtn) {
        this->setTabOrder(optBtn, fullBtn);
        this->setTabOrder(fullBtn, minBtn);
    } else
        this->setTabOrder(optBtn, minBtn);
    this->setTabOrder(minBtn, maxBtn);
    this->setTabOrder(maxBtn, closeBtn);
}

void TitleWidget::keyPressEvent(QKeyEvent *event)
{
    if (nullptr == m_pSw)
        return CustomWidget::keyPressEvent(event);

    if (event->key() == Qt::Key_Up && !event->isAutoRepeat()) {
        m_pSw->onPrevScale();
    } else if (event->key() == Qt::Key_Down && !event->isAutoRepeat()) {
        m_pSw->onNextScale();
    }

    return CustomWidget::keyPressEvent(event);
}

void TitleWidget::setBtnDisable(const bool &bAble)
{
    m_pThumbnailBtn->setDisabled(bAble);
    m_pSw->setDisabled(bAble);
}

void TitleWidget::onCurSheetChanged(DocSheet *sheet)
{
    m_curSheet = sheet;

    if (nullptr == m_curSheet || m_curSheet->fileType() == Dr::Unknown) {
        setBtnDisable(true);
        return;

    } else if (Dr::PDF == m_curSheet->fileType()) {
        if (m_curSheet->isUnLocked()) {
            setBtnDisable(false);
            m_pThumbnailBtn->setChecked(m_curSheet->operation().sidebarVisible);
        } else {
            setBtnDisable(true);
            m_pThumbnailBtn->setChecked(false);
        }
        m_pSw->setSheet(m_curSheet);

    } else if (Dr::DjVu == m_curSheet->fileType()) {
        m_pThumbnailBtn->setDisabled(false);
        m_pSw->setDisabled(false);

        m_pThumbnailBtn->setChecked(m_curSheet->operation().sidebarVisible);
        m_pSw->setSheet(m_curSheet);
    }
}

void TitleWidget::onThumbnailBtnClicked(bool checked)
{
    if (m_curSheet.isNull())
        return;

    m_pThumbnailBtn->setChecked(checked);
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


/**
 * @brief TitleWidget::setControlEnabled
 * 设置TitleWidget中空间的可用或者不可用
 * @param enable  true:可用      false:不可用
 */
void TitleWidget::setControlEnabled(const bool &enable)
{
    m_pThumbnailBtn->setChecked(false);
    m_pThumbnailBtn->setEnabled(enable);
    m_pSw->clear();
}
