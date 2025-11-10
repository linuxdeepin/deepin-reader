                                                                                                                                                                               // Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TitleWidget.h"
#include "ScaleWidget.h"
#include "DocSheet.h"
#include "MsgHeader.h"
#include "MainWindow.h"
#include "Application.h"
#include "ddlog.h"

#include <QHBoxLayout>

TitleWidget::TitleWidget(DWidget *parent)
    : BaseWidget(parent)
{
    qCDebug(appLog) << "Initializing TitleWidget...";
    m_pThumbnailBtn = new DIconButton(this);
    m_pThumbnailBtn->setObjectName("Thumbnails");
    m_pThumbnailBtn->setToolTip(tr("Thumbnails"));
    m_pThumbnailBtn->setCheckable(true);
    m_pThumbnailBtn->setDisabled(true);
    m_pThumbnailBtn->setIcon(QIcon::fromTheme(QString("dr_") + "thumbnails"));
    connect(m_pThumbnailBtn, SIGNAL(clicked(bool)), SLOT(onThumbnailBtnClicked(bool)));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(6, 0, 6, 0);
    hlayout->setSpacing(10);
    setLayout(hlayout);
    hlayout->addWidget(m_pThumbnailBtn);

    m_pSw = new ScaleWidget(this);
    m_pSw->setDisabled(true);
    hlayout->addWidget(m_pSw);
    hlayout->addStretch(1);

    DIconButton *decBtn = m_pSw->findChild<DIconButton *>("SP_DecreaseElement");
    decBtn->setEnabledCircle(true);
    DIconButton *incBtn = m_pSw->findChild<DIconButton *>("SP_IncreaseElement");
    incBtn->setEnabledCircle(true);

    DLineEdit *scaleEdit = m_pSw->findChild<DLineEdit *>("scaleEdit");

    DIconButton *optBtn = parent->findChild<DIconButton *>("DTitlebarDWindowOptionButton");
    DIconButton *fullBtn = parent->findChild<DIconButton *>("DTitlebarDWindowQuitFullscreenButton");
    DIconButton *minBtn = parent->findChild<DIconButton *>("DTitlebarDWindowMinButton");
    DIconButton *maxBtn = parent->findChild<DIconButton *>("DTitlebarDWindowMaxButton");
    DIconButton *closeBtn = parent->findChild<DIconButton *>("DTitlebarDWindowCloseButton");

    QList<QWidget *> orderlst;
    orderlst << m_pThumbnailBtn;
    orderlst << decBtn;
    orderlst << scaleEdit;
    orderlst << incBtn;
    orderlst << optBtn;
    if (fullBtn)
        orderlst << fullBtn;
    orderlst << minBtn;
    orderlst << maxBtn;
    orderlst << closeBtn;
    parent->setProperty("orderlist", QVariant::fromValue(orderlst));

    for (QWidget *widget : orderlst) {
        if (widget)
            widget->setFocusPolicy(Qt::TabFocus);
    }
#ifdef DTKWIDGET_CLASS_DSizeMode
    if (DGuiApplicationHelper::instance()->sizeMode() == DGuiApplicationHelper::NormalMode) {
            m_pThumbnailBtn->setFixedSize(QSize(36, 36));
            m_pThumbnailBtn->setIconSize(QSize(36, 36));
    } else {
            m_pThumbnailBtn->setFixedSize(QSize(24, 24));
            m_pThumbnailBtn->setIconSize(QSize(36, 36));
    }
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [=](DGuiApplicationHelper::SizeMode sizeMode) {
        if (sizeMode == DGuiApplicationHelper::NormalMode) {
                m_pThumbnailBtn->setFixedSize(QSize(36, 36));
                m_pThumbnailBtn->setIconSize(QSize(36, 36));
        } else {
                m_pThumbnailBtn->setFixedSize(QSize(24, 24));
                m_pThumbnailBtn->setIconSize(QSize(36, 36));
        }
    });
#else
        m_pThumbnailBtn->setFixedSize(QSize(36, 36));
        m_pThumbnailBtn->setIconSize(QSize(36, 36));
#endif
    qCDebug(appLog) << "TitleWidget initialized";
}

TitleWidget::~TitleWidget()
{
    // qCDebug(appLog) << "Destroying TitleWidget...";
}

void TitleWidget::keyPressEvent(QKeyEvent *event)
{
    // qCDebug(appLog) << "TitleWidget::keyPressEvent start - key:" << event->key();
    if (nullptr == m_pSw) {
        // qCDebug(appLog) << "Scale widget is null, delegating to base";
        return BaseWidget::keyPressEvent(event);
    }

    if (event->key() == Qt::Key_Up && !event->isAutoRepeat()) {
        // qCDebug(appLog) << "Up key pressed, previous scale";
        m_pSw->onPrevScale();
    } else if (event->key() == Qt::Key_Down && !event->isAutoRepeat()) {
        // qCDebug(appLog) << "Down key pressed, next scale";
        m_pSw->onNextScale();
    }

    // qCDebug(appLog) << "TitleWidget::keyPressEvent end";
    return BaseWidget::keyPressEvent(event);
}

void TitleWidget::setBtnDisable(const bool &bAble)
{
    qCDebug(appLog) << "Setting button disable:" << bAble;
    m_pThumbnailBtn->setDisabled(bAble);
    m_pSw->setDisabled(bAble);
}

void TitleWidget::onCurSheetChanged(DocSheet *sheet)
{
    qCDebug(appLog) << "Current sheet changed";
    m_curSheet = sheet;

    emit dApp->emitSheetChanged();

    if (nullptr == m_curSheet || m_curSheet->fileType() == Dr::Unknown) {
        qCWarning(appLog) << "Invalid sheet, disabling controls";
        setBtnDisable(true);
        return;
    }

    qCDebug(appLog) << "Setting up controls for file type:" << m_curSheet->fileType();
    if (Dr::PDF == m_curSheet->fileType()
            || Dr::DOCX == m_curSheet->fileType()
#ifdef XPS_SUPPORT_ENABLED
            || Dr::XPS == m_curSheet->fileType()
#endif
        ) {
        if (m_curSheet->opened()) {
            qCDebug(appLog) << "Document opened, enabling controls";
            setBtnDisable(false);
            m_pThumbnailBtn->setChecked(m_curSheet->operation().sidebarVisible);
        } else {
            qCDebug(appLog) << "Document not opened, disabling controls";
            setBtnDisable(true);
            m_pThumbnailBtn->setChecked(false);
        }
        m_pSw->setSheet(m_curSheet);
    } else if (Dr::DJVU == m_curSheet->fileType()) {
        qCDebug(appLog) << "DJVU document, setting up controls";
        m_pThumbnailBtn->setDisabled(false);
        m_pSw->setDisabled(false);
        m_pThumbnailBtn->setChecked(m_curSheet->operation().sidebarVisible);
        m_pSw->setSheet(m_curSheet);
    }
}

void TitleWidget::onThumbnailBtnClicked(bool checked)
{
    qCDebug(appLog) << "Thumbnail button clicked, checked:" << checked;
    if (m_curSheet.isNull()) {
        qCWarning(appLog) << "Thumbnail button clicked but no current sheet";
        return;
    }

    m_pThumbnailBtn->setChecked(checked);
    bool rl = m_pThumbnailBtn->isChecked();
    qCDebug(appLog) << "Setting sidebar visibility to:" << rl;
    m_curSheet->setSidebarVisible(rl);
    qCDebug(appLog) << "Thumbnail button clicked, checked:" << checked;
}

void TitleWidget::onFindOperation(const int &sAction)
{
    qCDebug(appLog) << "Finding operation:" << sAction;
    if (sAction == E_FIND_CONTENT) {
        qCDebug(appLog) << "Finding operation content";
        m_pThumbnailBtn->setChecked(true);
    } else if (sAction == E_FIND_EXIT) {
        qCDebug(appLog) << "Finding operation exit";
        if (m_curSheet) {
            bool close = m_curSheet->operation().sidebarVisible;
            m_pThumbnailBtn->setChecked(close);
        }
    }
    qCDebug(appLog) << "Finding operation end";
}

void TitleWidget::setControlEnabled(const bool &enable)
{
    qCDebug(appLog) << "Setting controls enabled:" << enable;
    m_pThumbnailBtn->setChecked(false);
    m_pThumbnailBtn->setEnabled(enable);
    m_pSw->clear();
    qCDebug(appLog) << "Setting controls enabled end";
}
