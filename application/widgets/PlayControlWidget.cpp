#include "PlayControlWidget.h"

#include <DPlatformWindowHandle>
#include <QTimer>
#include <QHBoxLayout>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

#include "utils/PublicFunction.h"
#include "utils/utils.h"
#include "docview/docummentproxy.h"
#include "CustomControl/CustomWidget.h"
#include "DocSheet.h"
#include "CentralDocPage.h"

PlayControlWidget::PlayControlWidget(DocSheet *sheet, DWidget *parnet)
    : DFloatingWidget(parnet), m_sheet(sheet)
{
    m_strObserverName = PLAY_CONTROL_WIDGET;
    m_bcanshow = false;
    m_bautoplaying = true;
    m_bfirstshow = true;
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);

    setBlurBackgroundEnabled(true);
    setFramRadius(18);

    m_ptimer = new QTimer(this);
    m_ptimer->setInterval(3000);
    initWidget();
    initConnections();
    adjustSize();

    slotUpdateTheme();

    dApp->m_pModelService->addObserver(this);
}

PlayControlWidget::~PlayControlWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

int PlayControlWidget::dealWithData(const int &, const QString &)
{
    return  MSG_NO_OK;
}

void PlayControlWidget::activeshow(int ix, int iy)
{
    if (m_bfirstshow) {
        m_bfirstshow = false;

        auto helper = m_sheet->getDocProxy();
        connect(helper, &DocummentProxy::signal_autoplaytoend, this, [this] {
            this->changePlayStatus();
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
}

DIconButton *PlayControlWidget::createBtn(const QString &strname)
{
    DIconButton *btn = new  DIconButton(this);
    btn->setObjectName(strname);
    int tW = 50;
    int tH = 50;
//    dApp->adaptScreenView(tW, tH);
    btn->setFixedSize(QSize(tW, tH));
    btn->setIcon(QIcon(Utils::renderSVG(PF::getImagePath(strname, Pri::g_icons), QSize(36, 36))));
    tW = 36;
    tH = 36;
//    dApp->adaptScreenView(tW, tH);
    btn->setIconSize(QSize(tW, tH));
    return  btn;
}

void PlayControlWidget::pagejump(const bool &bpre)
{
    auto helper = m_sheet->getDocProxy();
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

        m_sheet->pageJump(nCurPage);

        if (bSatus) {
            helper->setAutoPlaySlide(bSatus);
        }
    }
}

void PlayControlWidget::changePlayStatus()
{
    m_bautoplaying = !m_bautoplaying;
    if (m_bautoplaying)
        m_pbtnplay->setIcon(QIcon(Utils::renderSVG(PF::getImagePath("suspend_normal", Pri::g_icons), QSize(36, 36))));
    else {
        m_pbtnplay->setIcon(QIcon(Utils::renderSVG(PF::getImagePath("play_normal", Pri::g_icons), QSize(36, 36))));
    }
}

void PlayControlWidget::setSliderPath(const QString &strSliderPath)
{
    m_strSliderPath = strSliderPath;
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
    QList<DIconButton *> plist = this->findChildren<DIconButton *>();
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
    QJsonObject obj;
    obj.insert("type", "keyPress");
    obj.insert("key", KeyStr::g_space);
    QJsonDocument doc = QJsonDocument(obj);
    CentralDocPage *docPage = static_cast<CentralDocPage*>(parent());
    if(nullptr == docPage)
        return;

    docPage->OnAppMsgData(doc.toJson(QJsonDocument::Compact));
}

void PlayControlWidget::slotNextClicked()
{
    pagejump(false);
}

void PlayControlWidget::slotExitClicked()
{
    m_sheet->quitSlide();
}

//  按了键盘 上下左右
void PlayControlWidget::PageChangeByKey(const QString &sKey)
{
    if (sKey == KeyStr::g_space) {
        changePlayStatus();
    } else {
        if (sKey == KeyStr::g_up || sKey == KeyStr::g_pgup || sKey == KeyStr::g_left) {
            pagejump(true);
        } else {
            pagejump(false);
        }
    }
}
