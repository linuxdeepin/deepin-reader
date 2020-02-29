#include "PlayControlWidget.h"

#include <DPlatformWindowHandle>
#include <QTimer>
#include <QHBoxLayout>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

#include "business/FileDataManager.h"
#include "utils/PublicFunction.h"
#include "utils/utils.h"
#include "docview/docummentproxy.h"

#include "business/bridge/IHelper.h"

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

    m_pMsgList = {MSG_NOTIFY_KEY_PLAY_MSG};


    dApp->m_pModelService->addObserver(this);
}

PlayControlWidget::~PlayControlWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

int PlayControlWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return ConstantMsg::g_effective_res;
    }

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
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
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

    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)), SLOT(SlotDealWithData(const int &, const QString &)));
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

void PlayControlWidget::pagejump(const bool &bpre)
{
    auto helper = DocummentProxy::instance();
    if (helper) {
        bool bSatus = helper->getAutoPlaySlideStatu();
        if (bSatus) {
            helper->setAutoPlaySlide(false);
        }
        int nCurPage = helper->currentPageNo();
        if (bpre)
            nCurPage--;
        else
            nCurPage++;

        dApp->m_pHelper->qDealWithData(MSG_DOC_JUMP_PAGE, QString::number(nCurPage));

        if (bSatus) {
            helper->setAutoPlaySlide(bSatus);
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
    DFloatingWidget::leaveEvent(event);
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

void PlayControlWidget::SlotDealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_NOTIFY_KEY_PLAY_MSG) {
        __PageChangeByKey(msgContent);
    }
}

//  按了键盘 上下左右
void PlayControlWidget::__PageChangeByKey(const QString &sKey)
{
    if (sKey == KeyStr::g_up || sKey == KeyStr::g_pgup || sKey == KeyStr::g_left) {
        pagejump(true);
    } else {
        pagejump(false);
    }
}
