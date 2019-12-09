#include "PlayControlWidget.h"
#include "subjectObserver/MsgHeader.h"
#include "subjectObserver/ModuleHeader.h"
#include <DPlatformWindowHandle>
#include <QTimer>
#include <QHBoxLayout>

PlayControlWidget::PlayControlWidget(DWidget *parnet)
    : DFloatingWidget(parnet)
{
    m_bcanshow = false;
    m_bautoplaying = true;
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setBlurBackgroundEnabled(true);
    setFramRadius(18);
    setFixedSize(260, 80);//DFloatingWidget有问题设置尺寸比实际显示尺寸宽高小10

    m_ptimer = new QTimer(this);
    m_ptimer->setInterval(3000);
    initWidget();
    initConnections();

    m_pNotifySubject = NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }

    m_pMsgSubject = MsgSubject::getInstance();
    if (m_pMsgSubject) {
        m_pMsgSubject->removeObserver(this);
    }
}

PlayControlWidget::~PlayControlWidget()
{
    m_ptimer->stop();
    m_ptimer->deleteLater();
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }

    if (m_pMsgSubject) {
        m_pMsgSubject->removeObserver(this);
    }
}

int PlayControlWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_NOTIFY_KEY_MSG == msgType) {
        if (KeyStr::g_space == msgContent) {
            changePlayStatus();
        }
    }
    return  0;
}

void PlayControlWidget::sendMsg(const int &msgType, const QString &msgContent)
{
    m_pMsgSubject->sendMsg(msgType, msgContent);
}

void PlayControlWidget::notifyMsg(const int &msgType, const QString &msgContent)
{
    m_pMsgSubject->sendMsg(msgType, msgContent);
}

void PlayControlWidget::activeshow(int ix, int iy)
{
    if (m_ptimer->isActive())
        m_ptimer->stop();
    m_ptimer->start();
    move(ix, iy);
    raise();
    show();
}

void PlayControlWidget::killshow()
{
    m_bautoplaying = true;
    m_ptimer->stop();
    hide();
}

void PlayControlWidget::initWidget()
{
    QHBoxLayout *playout = new QHBoxLayout(this);
    playout->setContentsMargins(10, 10, 10, 10);
    playout->setSpacing(10);
    m_pbtnpre = createBtn(QString("previous_normal"));
    m_pbtnplay = createBtn(QString("suspend_normal"));
    m_pbtnnext = createBtn(QString("next_normal"));
    m_pbtnexit = createBtn(QString("exit_normal"));
    playout->addWidget(m_pbtnpre);
    playout->addWidget(m_pbtnplay);
    playout->addWidget(m_pbtnnext);
    playout->addWidget(m_pbtnexit);

}

void PlayControlWidget::initConnections()
{
    connect(m_ptimer, &QTimer::timeout, this, [this] {this->hide();});
    connect(m_pbtnpre, &DIconButton::clicked, this, &PlayControlWidget::slotPreClicked);
    connect(m_pbtnplay, &DIconButton::clicked, this, &PlayControlWidget::slotPlayClicked);
    connect(m_pbtnnext, &DIconButton::clicked, this, &PlayControlWidget::slotNextClicked);
    connect(m_pbtnexit, &DIconButton::clicked, this, &PlayControlWidget::slotExitClicked);
}

DIconButton *PlayControlWidget::createBtn(const QString &strname)
{
    DIconButton *btn = new  DIconButton(this);
    btn->setFixedSize(QSize(50, 50));
    btn->setIcon(QIcon(QString(":/icons/deepin/builtout/%1.svg").arg(strname)));
    btn->setIconSize(QSize(36, 36));
    return  btn;
}

void PlayControlWidget::changePlayStatus()
{
    m_bautoplaying = m_bautoplaying ? false : true;
    if (m_bautoplaying)
        m_pbtnplay->setIcon(QIcon(QString(":/icons/deepin/builtout/suspend_normal.svg")));
    else {
        m_pbtnplay->setIcon(QIcon(QString(":/icons/deepin/builtout/play_normal.svg")));
    }
    m_pbtnplay->setFixedSize(QSize(50, 50));
    m_pbtnplay->setIconSize(QSize(36, 36));
}

void PlayControlWidget::enterEvent(QEvent *event)
{
    m_ptimer->stop();
    show();
    DFloatingWidget::enterEvent(event);
}

void PlayControlWidget::leaveEvent(QEvent *event)
{
    m_ptimer->start();
    DFloatingWidget::enterEvent(event);
}

void PlayControlWidget::slotUpdateTheme()
{


}

void PlayControlWidget::slotPreClicked()
{
    notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_pgup);
}

void PlayControlWidget::slotPlayClicked()
{

    changePlayStatus();
    notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_space);
}

void PlayControlWidget::slotNextClicked()
{
    notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_pgdown);
}

void PlayControlWidget::slotExitClicked()
{
    notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_esc);
    m_ptimer->stop();
    hide();
}
