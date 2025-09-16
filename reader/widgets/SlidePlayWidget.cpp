// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SlidePlayWidget.h"
#include "Utils.h"
#include "ddlog.h"
#include <QDebug>

#include <DPlatformWindowHandle>
#include <DWidget>

#include <QHBoxLayout>
#include <QDBusInterface>
#include <QDBusConnection>
#include <DGuiApplicationHelper>
#include <DSysInfo>

DCORE_USE_NAMESPACE

SlidePlayWidget::SlidePlayWidget(QWidget *parent) : DFloatingWidget(parent)
{
    qCDebug(appLog) << "SlidePlayWidget created, parent:" << parent;
    initControl();
}

void SlidePlayWidget::initControl()
{
    qCDebug(appLog) << "Initializing slide play controls";
    setBlurBackgroundEnabled(true);
    setFramRadius(18);

    m_timer.setInterval(3000);
    m_timer.start();
    connect(&m_timer, &QTimer::timeout, this, &SlidePlayWidget::onTimerout);

    QHBoxLayout *playout = new QHBoxLayout;
    setContentsMargins(0, 0, 0, 0);
    playout->setContentsMargins(10, 10, 10, 10);
    playout->setSpacing(10);

    m_preBtn  = createBtn(QString("previous_normal"));
    DIconButton *pbtnplay = createBtn(QString("play_normal"));
    m_nextBtn = createBtn(QString("next_normal"));
    DIconButton *pbtnexit = createBtn(QString("exit_normal"));
    m_playBtn = pbtnplay;

    playout->addWidget(m_preBtn);
    playout->addWidget(pbtnplay);
    playout->addWidget(m_nextBtn);
    playout->addWidget(pbtnexit);

    DWidget *pwidget = new DWidget;
    pwidget->setLayout(playout);
    this->setWidget(pwidget);

    connect(m_preBtn,  &DIconButton::clicked, this, &SlidePlayWidget::onPreClicked);
    connect(pbtnplay, &DIconButton::clicked, this, &SlidePlayWidget::onPlayClicked);
    connect(m_nextBtn, &DIconButton::clicked, this, &SlidePlayWidget::onNextClicked);
    connect(pbtnexit, &DIconButton::clicked, this, &SlidePlayWidget::onExitClicked);

    setFocusPolicy(Qt::NoFocus);
    m_preBtn->setFocusPolicy(Qt::NoFocus);
    pbtnplay->setFocusPolicy(Qt::NoFocus);
    m_nextBtn->setFocusPolicy(Qt::NoFocus);
    pbtnexit->setFocusPolicy(Qt::NoFocus);
#ifdef DTKWIDGET_CLASS_DSizeMode
    if (DGuiApplicationHelper::instance()->sizeMode() == DGuiApplicationHelper::NormalMode) {
        qCDebug(appLog) << "Normal mode";
        m_preBtn->setFixedSize(50, 50);
        m_preBtn->setIconSize(QSize(36, 36));
        m_playBtn->setFixedSize(50, 50);
        m_playBtn->setIconSize(QSize(36, 36));
        m_nextBtn->setFixedSize(50, 50);
        m_nextBtn->setIconSize(QSize(36, 36));
        pbtnexit->setFixedSize(50, 50);
        pbtnexit->setIconSize(QSize(36, 36));
    } else {
        qCDebug(appLog) << "Compact mode";
        m_preBtn->setFixedSize(36, 36);
        m_preBtn->setIconSize(QSize(24, 24));
        m_playBtn->setFixedSize(36, 36);
        m_playBtn->setIconSize(QSize(24, 24));
        m_nextBtn->setFixedSize(36, 36);
        m_nextBtn->setIconSize(QSize(24, 24));
        pbtnexit->setFixedSize(36, 36);
        pbtnexit->setIconSize(QSize(24, 24));
    }
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [=](DGuiApplicationHelper::SizeMode sizeMode) {
        if (sizeMode == DGuiApplicationHelper::NormalMode) {
            qCDebug(appLog) << "Normal mode";
            m_preBtn->setFixedSize(50, 50);
            m_preBtn->setIconSize(QSize(36, 36));
            m_playBtn->setFixedSize(50, 50);
            m_playBtn->setIconSize(QSize(36, 36));
            m_nextBtn->setFixedSize(50, 50);
            m_nextBtn->setIconSize(QSize(36, 36));
            pbtnexit->setFixedSize(50, 50);
            pbtnexit->setIconSize(QSize(36, 36));
        } else {
            qCDebug(appLog) << "Compact mode";
            m_preBtn->setFixedSize(36, 36);
            m_preBtn->setIconSize(QSize(24, 24));
            m_playBtn->setFixedSize(36, 36);
            m_playBtn->setIconSize(QSize(24, 24));
            m_nextBtn->setFixedSize(36, 36);
            m_nextBtn->setIconSize(QSize(24, 24));
            pbtnexit->setFixedSize(36, 36);
            pbtnexit->setIconSize(QSize(24, 24));
        }
        if(isVisible()) {
            hide();
            showControl();
        }
    });
#endif
}

void SlidePlayWidget::showControl()
{
    qCDebug(appLog) << "Showing slide play controls";
    m_timer.start();
    this->show();
}

void SlidePlayWidget::onTimerout()
{
    qCDebug(appLog) << "Timer timeout";
    this->hide();
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void SlidePlayWidget::enterEvent(QEvent *event)
{
    qCDebug(appLog) << "Enter event";
    m_timer.stop();
    DFloatingWidget::enterEvent(event);
}
#else
void SlidePlayWidget::enterEvent(QEnterEvent *event)
{
    qCDebug(appLog) << "Enter event";
    m_timer.stop();
    DFloatingWidget::enterEvent(event);
}
#endif

void SlidePlayWidget::leaveEvent(QEvent *event)
{
    qCDebug(appLog) << "Leave event";
    m_timer.start();
    DFloatingWidget::leaveEvent(event);
}

DIconButton *SlidePlayWidget::createBtn(const QString &strname)
{
    qCDebug(appLog) << "Creating button:" << strname;
    DIconButton *btn = new  DIconButton(this);
    btn->setObjectName(strname);
    btn->setFixedSize(50, 50);
    btn->setIcon(/*PF::getIcon*/QIcon::fromTheme(QString("dr_") + strname));
    btn->setIconSize(QSize(36, 36));
    btn->setWindowFlag(Qt::FramelessWindowHint);
    return  btn;
}

void SlidePlayWidget::setPlayStatus(bool play)
{
    qCDebug(appLog) << "Setting play status:" << play;
    m_autoPlay = play;
    playStatusChanged();
}

void SlidePlayWidget::updateProcess(int cur, int total)
{
    qCDebug(appLog) << "Updating slide position, current:" << cur << ", total:" << total;
    /* 幻灯片放映时
     * cur:当前页、total总页数
     * 当cur:0, total:3
     * 已到第一页时，再次点击上一页按钮，cur=-1,则设置preBtn为disable
     *
     * 当cur:2, total:3
     * 已到最后一页时，再次点击下一页按钮，cur=3,则设置nextBtn为disable
     */
    if(cur <= -1) {
        qCDebug(appLog) << "Updating slide position, current is less than -1";
        if(m_preBtn->isEnabled())
            Notify(tr("It is the first page"));
        m_preBtn->setEnabled(false);
        m_nextBtn->setEnabled(true);
    } else if(cur >=  total) {
        qCDebug(appLog) << "Updating slide position, current is greater than total";
        if(m_nextBtn->isEnabled())
            Notify(tr("It is the last page"));
        m_preBtn->setEnabled(true);
        m_nextBtn->setEnabled(false);
        setPlayStatus(false);
    } else {
        qCDebug(appLog) << "Updating slide position, current is between 0 and total";
        m_preBtn->setEnabled(true);
        m_nextBtn->setEnabled(true);
    }
}

void SlidePlayWidget::Notify(const QString &text)
{
    qCDebug(appLog) << "Showing notification:" << text;
    if(nullptr == m_dbusNotify) {
        qCDebug(appLog) << "Initializing dbus notify";
        QString serviceName = "com.deepin.dde.Notification";
        QString servicePath = "/com/deepin/dde/Notification";
        QString serviceInterface = "com.deepin.dde.Notification";
        int osMajor = DSysInfo::majorVersion().toInt();
        if (osMajor >= 23) {
            serviceName = "org.deepin.dde.Notification1";
            servicePath = "/org/deepin/dde/Notification1";
            serviceInterface = "org.deepin.dde.Notification1";
        }

        m_dbusNotify = new QDBusInterface(serviceName,
                                          servicePath,
                                          serviceInterface,
                                          QDBusConnection::sessionBus(),
                                          this);
    }
    //初始化Notify 七个参数
    QString appname("deepin-reader");
    uint replaces_id = 0;
    QString appicon("deepin-reader");
    QString title = "";
    QString body = text;
    QStringList actionlist;
    QVariantMap hints;
    int timeout = 3000;
    m_dbusNotify->call("Notify", appname, replaces_id, appicon, title, body, actionlist, hints, timeout);
    qCDebug(appLog) << "Showing notification end";
}

bool SlidePlayWidget::getPlayStatus()
{
    qCDebug(appLog) << "Getting play status";
    return m_autoPlay;
}

void SlidePlayWidget::onPreClicked()
{
    qCDebug(appLog) << "Previous button clicked";
    emit signalPreBtnClicked();
}

void SlidePlayWidget::onPlayClicked()
{
    qCDebug(appLog) << "Play button clicked, new state:" << !m_autoPlay;
    m_autoPlay = !m_autoPlay;
    playStatusChanged();
}

void SlidePlayWidget::onNextClicked()
{
    qCDebug(appLog) << "Next button clicked";
    emit signalNextBtnClicked();
}

void SlidePlayWidget::onExitClicked()
{
    qCDebug(appLog) << "Exit button clicked";
    emit signalExitBtnClicked();
}

void SlidePlayWidget::playStatusChanged()
{
    qCDebug(appLog) << "Play status changed to:" << m_autoPlay;
    if (m_autoPlay) {
        qCDebug(appLog) << "Play status changed to true";
        m_playBtn->setIcon(QIcon::fromTheme(QString("dr_") + "suspend_normal"));
    } else {
        qCDebug(appLog) << "Play status changed to false";
        m_playBtn->setIcon(QIcon::fromTheme(QString("dr_") + "play_normal"));
    }
    emit signalPlayBtnClicked();
    qCDebug(appLog) << "Play status changed end";
}

void SlidePlayWidget::mousePressEvent(QMouseEvent *)
{
    // qCDebug(appLog) << "Mouse press event";
    //Nottodo
}
