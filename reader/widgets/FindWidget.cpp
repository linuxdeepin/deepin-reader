// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FindWidget.h"
#include "BaseWidget.h"
#include "DocSheet.h"
#include "Utils.h"
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
    qDebug() << "FindWidget created, parent:" << parent;
    setMinimumSize(QSize(414, 60));

    setBlurBackgroundEnabled(true);

    initWidget();

    m_parentWidget->installEventFilter(this);
    this->setVisible(false);
}

FindWidget::~FindWidget()
{
    // qDebug() << "FindWidget destroyed";
}

void FindWidget::setDocSheet(DocSheet *sheet)
{
    qDebug() << "setDocSheet";
    m_docSheet = sheet;
}

void FindWidget::updatePosition()
{
    qDebug() << "Updating find widget position, yOffset:" << m_yOff;
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    this->move(screenGeometry.width() - this->width() - 10, m_yOff + 10);

    this->show();
    this->raise();
}

void FindWidget::setSearchEditFocus()
{
    qDebug() << "setSearchEditFocus";
    m_pSearchEdit->lineEdit()->setFocus();
}

void FindWidget::onSearchStop()
{
    qDebug() << "Search stopped, last search text:" << (m_lastSearchText.isEmpty() ? "empty" : "non-empty");
    m_lastSearchText.clear();

    m_findPrevButton->setDisabled(true);
    m_findNextButton->setDisabled(true);

    setEditAlert(0);

    if (m_docSheet)
        m_docSheet->stopSearch();
}

void FindWidget::onSearchStart()
{
    qDebug() << "Search started, text:" << (m_pSearchEdit->text().isEmpty() ? "empty" : "non-empty");
    QString searchText = m_pSearchEdit->text().trimmed();

    if ((searchText != "") && (m_lastSearchText != searchText)) {
        qDebug() << "searchText != \"\" && m_lastSearchText != searchText";
        m_lastSearchText = searchText;
        setEditAlert(0);
        m_docSheet->startSearch(searchText);
        m_findPrevButton->setDisabled(false);
        m_findNextButton->setDisabled(false);
    }
}

void FindWidget::slotFindNextBtnClicked()
{
    qDebug() << "Find next result triggered";
    if (m_docSheet)
        m_docSheet->jumpToNextSearchResult();
}

void FindWidget::slotFindPrevBtnClicked()
{
    qDebug() << "Find previous result triggered";
    if (m_docSheet)
        m_docSheet->jumpToPrevSearchResult();
}

void FindWidget::onTextChanged()
{
    qDebug() << "onTextChanged";
    if (m_pSearchEdit->text().isEmpty()) {
        qDebug() << "m_pSearchEdit->text().isEmpty()";
        setEditAlert(0);
    }
}

void FindWidget::initWidget()
{
    qDebug() << "Initializing find widget controls";
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
        qDebug() << "DGuiApplicationHelper::NormalMode";
        m_pSearchEdit->setFixedSize(QSize(270, 36));
        m_findPrevButton->setFixedSize(QSize(36, 36));
        m_findNextButton->setFixedSize(QSize(36, 36));
        setFixedSize(422, 60);
    } else {
        qDebug() << "DGuiApplicationHelper::SmallMode";
        m_pSearchEdit->setFixedSize(QSize(270, 24));
        m_findPrevButton->setFixedSize(QSize(24, 24));
        m_findNextButton->setFixedSize(QSize(24, 24));
        setFixedSize(398, 45);
    }
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [=](DGuiApplicationHelper::SizeMode sizeMode) {
        if (sizeMode == DGuiApplicationHelper::NormalMode) {
            qDebug() << "DGuiApplicationHelper::NormalMode";
            m_pSearchEdit->setFixedSize(QSize(270, 36));
            m_findPrevButton->setFixedSize(QSize(36, 36));
            m_findNextButton->setFixedSize(QSize(36, 36));
            setFixedSize(422, 60);
        } else {
            qDebug() << "DGuiApplicationHelper::SmallMode";
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
    qDebug() << "Find widget controls initialized";
}

void FindWidget::setEditAlert(const int &iFlag)
{
    qDebug() << "setEditAlert";
    if (m_pSearchEdit) {
        bool bAlert = (iFlag == 1 ? true : false);

        if (m_pSearchEdit->text().isEmpty())
            bAlert = false;

        m_pSearchEdit->setAlert(bAlert);
    }
}

void FindWidget::setYOff(int yOff)
{
    qDebug() << "setYOff";
    m_yOff = yOff;
}

void FindWidget::onCloseBtnClicked()
{
    qDebug() << "Find widget close button clicked";
    onSearchStop();

    m_pSearchEdit->clear();

    this->hide();

    this->deleteLater();
}

void FindWidget::keyPressEvent(QKeyEvent *event)
{
    // qDebug() << "keyPressEvent";
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
        // qDebug() << "keyPressEvent Up or Down";
        //这里不过滤掉的话,事件会跑到Browser上
        return;
    }

    DFloatingWidget::keyPressEvent(event);
}

bool FindWidget::eventFilter(QObject *watched, QEvent *event)
{
    // qDebug() << "eventFilter";
    if(watched == m_parentWidget && event->type() == QEvent::Resize) {
        // qDebug() << "eventFilter Resize";
        updatePosition();
    }
    // qDebug() << "eventFilter end";
    return DFloatingWidget::eventFilter(watched, event);
}
