// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FindWidget.h"
#include "BaseWidget.h"
#include "DocSheet.h"
#include "Utils.h"
#include "ddlog.h"
#include <QDebug>

#include <DDialogCloseButton>
#include <DIconButton>

#include <QHBoxLayout>
#include <QScreen>
#include <DGuiApplicationHelper>

FindWidget::FindWidget(DWidget *parent)
    : DFloatingWidget(parent)
    , m_parentWidget(parent)
{
    qCDebug(appLog) << "FindWidget created, parent:" << parent;
    setMinimumSize(QSize(414, 60));

    setBlurBackgroundEnabled(true);

    initWidget();

    m_parentWidget->installEventFilter(this);
    this->setVisible(false);
}

FindWidget::~FindWidget()
{
    // qCDebug(appLog) << "FindWidget destroyed";
}

void FindWidget::setDocSheet(DocSheet *sheet)
{
    qCDebug(appLog) << "setDocSheet";
    m_docSheet = sheet;
}

void FindWidget::updatePosition()
{
    qCDebug(appLog) << "Updating find widget position, yOffset:" << m_yOff;
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    this->move(screenGeometry.width() - this->width() - 10, m_yOff + 10);

    this->show();
    this->raise();
}

void FindWidget::setSearchEditFocus()
{
    qCDebug(appLog) << "setSearchEditFocus";
    m_pSearchEdit->lineEdit()->setFocus();
}

void FindWidget::onSearchStop()
{
    qCDebug(appLog) << "Search stopped, last search text:" << (m_lastSearchText.isEmpty() ? "empty" : "non-empty");
    m_lastSearchText.clear();

    m_findPrevButton->setDisabled(true);
    m_findNextButton->setDisabled(true);

    setEditAlert(0);

    if (m_docSheet)
        m_docSheet->stopSearch();
}

void FindWidget::onSearchStart()
{
    qCDebug(appLog) << "Search started, text:" << (m_pSearchEdit->text().isEmpty() ? "empty" : "non-empty");
    QString searchText = m_pSearchEdit->text().trimmed();

    if ((searchText != "") && (m_lastSearchText != searchText)) {
        qCDebug(appLog) << "searchText != \"\" && m_lastSearchText != searchText";
        m_lastSearchText = searchText;
        setEditAlert(0);
        m_docSheet->startSearch(searchText);
        m_findPrevButton->setDisabled(false);
        m_findNextButton->setDisabled(false);
    }
}

void FindWidget::slotFindNextBtnClicked()
{
    qCDebug(appLog) << "Find next result triggered";
    if (m_docSheet)
        m_docSheet->jumpToNextSearchResult();
}

void FindWidget::slotFindPrevBtnClicked()
{
    qCDebug(appLog) << "Find previous result triggered";
    if (m_docSheet)
        m_docSheet->jumpToPrevSearchResult();
}

void FindWidget::onTextChanged()
{
    qCDebug(appLog) << "onTextChanged";
    if (m_pSearchEdit->text().isEmpty()) {
        qCDebug(appLog) << "m_pSearchEdit->text().isEmpty()";
        setEditAlert(0);
    }
}

void FindWidget::initWidget()
{
    qCDebug(appLog) << "Initializing find widget controls";
    m_pSearchEdit = new DSearchEdit(this);
    m_pSearchEdit->setObjectName("findSearchEdit_P");
    m_pSearchEdit->lineEdit()->setObjectName("findSearchEdit");
    m_pSearchEdit->lineEdit()->setFocusPolicy(Qt::StrongFocus);
    connect(m_pSearchEdit, &DSearchEdit::returnPressed, this, &FindWidget::onSearchStart);
    connect(m_pSearchEdit, &DSearchEdit::textChanged, this, &FindWidget::onTextChanged);
    connect(m_pSearchEdit, &DSearchEdit::searchAborted, this, &FindWidget::onSearchStop);

    m_findPrevButton = new DIconButton(DStyle::SP_ArrowUp, this);
    m_findPrevButton->setObjectName("SP_ArrowUpBtn");
    m_findPrevButton->setToolTip(tr("Previous"));
    m_findPrevButton->setIconSize(QSize(12, 12));
    m_findPrevButton->setDisabled(true);
    connect(m_findPrevButton, &DIconButton::clicked, this, &FindWidget::slotFindPrevBtnClicked);

    m_findNextButton = new DIconButton(DStyle::SP_ArrowDown, this);
    m_findNextButton->setObjectName("SP_ArrowDownBtn");
    m_findNextButton->setToolTip(tr("Next"));
    m_findNextButton->setIconSize(QSize(12, 12));
    m_findNextButton->setDisabled(true);
    connect(m_findNextButton, &DIconButton::clicked, this, &FindWidget::slotFindNextBtnClicked);

    DDialogCloseButton *closeButton = new DDialogCloseButton(this);
    closeButton->setObjectName("closeButton");
    connect(closeButton, &DDialogCloseButton::clicked, this, &FindWidget::onCloseBtnClicked);

    auto layout = new QHBoxLayout;
    layout->setContentsMargins(8, 0, 6, 0);
    layout->addWidget(m_pSearchEdit);
    layout->addWidget(m_findPrevButton);
    layout->addWidget(m_findNextButton);
    layout->addWidget(closeButton);
    this->setLayout(layout);
    closeButton->setIconSize(QSize(28, 28));
    closeButton->setFixedSize(QSize(30, 30));
    m_pSearchEdit->setWindowFlag(Qt::FramelessWindowHint);
    m_findPrevButton->setWindowFlag(Qt::FramelessWindowHint);
    m_findNextButton->setWindowFlag(Qt::FramelessWindowHint);
    closeButton->setWindowFlag(Qt::FramelessWindowHint);

#ifdef DTKWIDGET_CLASS_DSizeMode
    if (DGuiApplicationHelper::instance()->sizeMode() == DGuiApplicationHelper::NormalMode) {
        qCDebug(appLog) << "DGuiApplicationHelper::NormalMode";
        m_pSearchEdit->setFixedSize(QSize(270, 36));
        m_findPrevButton->setFixedSize(QSize(36, 36));
        m_findNextButton->setFixedSize(QSize(36, 36));
        setFixedSize(422, 60);
    } else {
        qCDebug(appLog) << "DGuiApplicationHelper::SmallMode";
        m_pSearchEdit->setFixedSize(QSize(270, 24));
        m_findPrevButton->setFixedSize(QSize(24, 24));
        m_findNextButton->setFixedSize(QSize(24, 24));
        setFixedSize(398, 45);
    }
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [=](DGuiApplicationHelper::SizeMode sizeMode) {
        if (sizeMode == DGuiApplicationHelper::NormalMode) {
            qCDebug(appLog) << "DGuiApplicationHelper::NormalMode";
            m_pSearchEdit->setFixedSize(QSize(270, 36));
            m_findPrevButton->setFixedSize(QSize(36, 36));
            m_findNextButton->setFixedSize(QSize(36, 36));
            setFixedSize(422, 60);
        } else {
            qCDebug(appLog) << "DGuiApplicationHelper::SmallMode";
            m_pSearchEdit->setFixedSize(QSize(270, 24));
            m_findPrevButton->setFixedSize(QSize(24, 24));
            m_findNextButton->setFixedSize(QSize(24, 24));
            setFixedSize(398, 45);
        }
    });
#else
    m_pSearchEdit->setFixedSize(QSize(270, 36));
    m_findPrevButton->setFixedSize(QSize(36, 36));
    m_findPrevButton->setIconSize(QSize(12, 12));
    m_findNextButton->setFixedSize(QSize(36, 36));
    m_findNextButton->setIconSize(QSize(12, 12));
    setFixedSize(422, 60);
#endif
    Utils::setObjectNoFocusPolicy(this);
    qCDebug(appLog) << "Find widget controls initialized";
}

void FindWidget::setEditAlert(const int &iFlag)
{
    qCDebug(appLog) << "setEditAlert";
    if (m_pSearchEdit) {
        bool bAlert = (iFlag == 1 ? true : false);

        if (m_pSearchEdit->text().isEmpty())
            bAlert = false;

        m_pSearchEdit->setAlert(bAlert);
    }
}

void FindWidget::setYOff(int yOff)
{
    qCDebug(appLog) << "setYOff";
    m_yOff = yOff;
}

void FindWidget::onCloseBtnClicked()
{
    qCDebug(appLog) << "Find widget close button clicked";
    onSearchStop();

    m_pSearchEdit->clear();

    this->hide();

    this->deleteLater();
}

void FindWidget::keyPressEvent(QKeyEvent *event)
{
    // qCDebug(appLog) << "keyPressEvent";
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
        // qCDebug(appLog) << "keyPressEvent Up or Down";
        //这里不过滤掉的话,事件会跑到Browser上
        return;
    }

    DFloatingWidget::keyPressEvent(event);
}

bool FindWidget::eventFilter(QObject *watched, QEvent *event)
{
    // qCDebug(appLog) << "eventFilter";
    if(watched == m_parentWidget && event->type() == QEvent::Resize) {
        // qCDebug(appLog) << "eventFilter Resize";
        updatePosition();
    }
    // qCDebug(appLog) << "eventFilter end";
    return DFloatingWidget::eventFilter(watched, event);
}
