#include "window.h"
#include <DTitlebar>
#include <DWidget>
#include <DPushButton>

Window::Window(DMainWindow *parent)
    : DMainWindow(parent)
{
    initUI();

    initTitlebar();
}

Window::~Window()
{
    // We don't need clean pointers because application has exit here.
}

void Window::initUI()
{
    m_centralWidget = new MainWidget();
    setCentralWidget(m_centralWidget);

    m_titleWidget = new TitleWidget();
    connect(m_titleWidget, SIGNAL(sendThumbnailState(const bool&)), m_centralWidget, SLOT(setSliderState(const bool&)));

    titlebar()->addWidget(m_titleWidget, Qt::AlignLeft);
}

void Window::initConnections()
{

}

void Window::initTitlebar()
{
    titlebar()->setTitle(tr("123"));
}
