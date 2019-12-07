#include "PlayControlWidget.h"
#include <DPlatformWindowHandle>
#include <QTimer>
#include <QHBoxLayout>

PlayControlWidget::PlayControlWidget(CustomWidget *parnet)
    : CustomWidget("PlayControlWidget", parnet)
{
    setWindowFlags(Qt::ToolTip);
    setFixedSize(250, 70);
    DPlatformWindowHandle handle(this);
    int radius = 18;
    handle.setWindowRadius(radius);

    //  setAttribute(Qt::WA_TranslucentBackground);

    // setWindowOpacity(0.98);
//    QPalette pa = palette();
//    pa.setColor(QPalette::Background, QColor(230, 130, 230, 30));
//    setPalette(pa);
//    setAutoFillBackground(true);

    m_ptimer = new QTimer(this);
    m_ptimer->setInterval(3000);
    initWidget();
    initConnections();
}

PlayControlWidget::~PlayControlWidget()
{
    m_ptimer->stop();
    m_ptimer->deleteLater();
}

int PlayControlWidget::dealWithData(const int &, const QString &)
{

    return  0;
}

void PlayControlWidget::activeshow()
{
    if (m_ptimer->isActive())
        m_ptimer->stop();
    m_ptimer->start();
    raise();
    show();
}

void PlayControlWidget::killshow()
{
    m_ptimer->stop();
    hide();
}

void PlayControlWidget::initWidget()
{
    QHBoxLayout *playout = new QHBoxLayout(this);
    playout->setContentsMargins(10, 10, 10, 10);
    playout->setSpacing(10);
    m_pbtnpre = createBtn();
    m_pbtnplay = createBtn();
    m_pbtnnext = createBtn();
    m_pbtnexit = createBtn();
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

DIconButton *PlayControlWidget::createBtn(const QString &strobjname)
{
    DIconButton *btn = new  DIconButton(QStyle::SP_ArrowBack, this);
    btn->setFixedSize(QSize(50, 50));
    return  btn;
}

void PlayControlWidget::showEvent(QShowEvent *event)
{
//    if (m_ptimer->isActive())
//        m_ptimer->stop();
//    m_ptimer->start();
}

void PlayControlWidget::hideEvent(QHideEvent *hideEvent)
{
//    m_ptimer->stop();
//    hide();
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
