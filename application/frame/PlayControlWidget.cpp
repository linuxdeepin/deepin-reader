#include "PlayControlWidget.h"
#include "subjectObserver/MsgHeader.h"
#include "subjectObserver/ModuleHeader.h"
#include "utils/PublicFunction.h"
#include "utils/utils.h"
#include "frame/DocummentFileHelper.h"
#include <DPlatformWindowHandle>
#include <QTimer>
#include <QHBoxLayout>
#include <QtDebug>

PlayControlWidget::PlayControlWidget(DWidget *parnet)
    : DFloatingWidget(parnet)
{
    m_strObserverName = "PlayControlWidget";
    m_bcanshow = false;
    m_bautoplaying = true;
    m_bfirstshow = true;
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setBlurBackgroundEnabled(true);
    setFramRadius(18);

    m_ptimer = new QTimer(this);
    m_ptimer->setInterval(3000);
    initWidget();
    initConnections();
    adjustSize();

    m_pNotifySubject = g_NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

PlayControlWidget::~PlayControlWidget()
{
    m_ptimer->stop();
    m_ptimer->deleteLater();
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

int PlayControlWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_NOTIFY_KEY_MSG == msgType) {
        if (KeyStr::g_space == msgContent) {
            changePlayStatus();
        }
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }
    return  0;
}

void PlayControlWidget::sendMsg(const int &msgType, const QString &msgContent)
{
    notifyMsg(msgType, msgContent);
}

void PlayControlWidget::notifyMsg(const int &msgType, const QString &msgContent)
{
    if (m_pNotifySubject) {
        m_pNotifySubject->notifyMsg(msgType, msgContent);
    }
}

void PlayControlWidget::activeshow(int ix, int iy)
{
    if (m_bfirstshow) {
        m_bfirstshow = false;
        connect(DocummentProxy::instance(), &DocummentProxy::signal_autoplaytoend, this, [this] {
            this->changePlayStatus(); qDebug() << "$$$%%%%$$####%$";
        });
    }
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
    m_pbtnplay->setIcon(QIcon(Utils::renderSVG(PF::getImagePath("suspend_normal", Pri::g_icons), QSize(36, 36))));
    m_ptimer->stop();
    hide();
}

void PlayControlWidget::initWidget()
{
    QHBoxLayout *playout = new QHBoxLayout;
    playout->setContentsMargins(10, 10, 10, 10);
    playout->setSpacing(10);
    m_pbtnpre = createBtn(QString("previous_normal"));

    m_pbtnplay = createBtn(QString("suspend_normal"));
    m_pbtnplay->setFixedSize(QSize(50, 50));
    m_pbtnplay->setIconSize(QSize(36, 36));

    m_pbtnnext = createBtn(QString("next_normal"));
    m_pbtnexit = createBtn(QString("exit_normal"));

    playout->addWidget(m_pbtnpre);
    playout->addWidget(m_pbtnplay);
    playout->addWidget(m_pbtnnext);
    playout->addWidget(m_pbtnexit);

    DWidget *pwidget = new DWidget;
    pwidget->setLayout(playout);
    this->setWidget(pwidget);
}

void PlayControlWidget::initConnections()
{
    connect(m_ptimer, &QTimer::timeout, this, [this] {this->hide();});
    connect(m_pbtnpre, &DIconButton::clicked, this, &PlayControlWidget::slotPreClicked);
    connect(m_pbtnplay, &DIconButton::clicked, this, &PlayControlWidget::slotPlayClicked);
    connect(m_pbtnnext, &DIconButton::clicked, this, &PlayControlWidget::slotNextClicked);
    connect(m_pbtnexit, &DIconButton::clicked, this, &PlayControlWidget::slotExitClicked);
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
}

DIconButton *PlayControlWidget::createBtn(const QString &strname)
{
    DIconButton *btn = new  DIconButton(this);
    btn->setObjectName(strname);
    btn->setFixedSize(QSize(50, 50));
    btn->setIcon(QIcon(Utils::renderSVG(PF::getImagePath(strname, Pri::g_icons), QSize(36, 36))));
    btn->setIconSize(QSize(36, 36));
    return  btn;
}

void PlayControlWidget::pagejump(bool bpre)
{
    auto helper = DocummentFileHelper::instance();
    if (helper) {
        bool bstart = false;
        if (helper->getAutoPlaySlideStatu()) {
            helper->setAutoPlaySlide(false);
            bstart = true;
        }
        int nCurPage = helper->currentPageNo();
        if (bpre)
            nCurPage--;
        else
            nCurPage++;

        notifyMsg(MSG_DOC_JUMP_PAGE, QString::number(nCurPage));

        if (bstart) {
            helper->setAutoPlaySlide(true);
            bstart = false;
        }
    }
}

void PlayControlWidget::changePlayStatus()
{
    m_bautoplaying = m_bautoplaying ? false : true;
    if (m_bautoplaying)
        m_pbtnplay->setIcon(QIcon(Utils::renderSVG(PF::getImagePath("suspend_normal", Pri::g_icons), QSize(36, 36))));
    else {
        m_pbtnplay->setIcon(QIcon(Utils::renderSVG(PF::getImagePath("play_normal", Pri::g_icons), QSize(36, 36))));
    }
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
    QList<DIconButton *> plist = findChildren<DIconButton *>();
    foreach (DIconButton *btn, plist) {
        if (btn == m_pbtnplay) {
            if (m_bautoplaying) {
                m_pbtnplay->setIcon(QIcon(Utils::renderSVG(PF::getImagePath("suspend_normal", Pri::g_icons), QSize(36, 36))));
            } else {
                m_pbtnplay->setIcon(QIcon(Utils::renderSVG(PF::getImagePath("play_normal", Pri::g_icons), QSize(36, 36))));
            }
        } else {
            btn->setIcon(QIcon(Utils::renderSVG(PF::getImagePath(btn->objectName(), Pri::g_icons), QSize(36, 36))));
        }
    }
}

void PlayControlWidget::slotPreClicked()
{
    pagejump(true);
}

void PlayControlWidget::slotPlayClicked()
{
    notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_space);
}

void PlayControlWidget::slotNextClicked()
{
    pagejump(false);
}

void PlayControlWidget::slotExitClicked()
{
    notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_esc);
    m_ptimer->stop();
    hide();
}
