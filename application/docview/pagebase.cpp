#include "pagebase.h"

ThreadLoadMagnifierCache::ThreadLoadMagnifierCache()
{
//    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    m_page = nullptr;
    restart = false;
    m_width = 0;
    m_height = 0;
}

void ThreadLoadMagnifierCache::setRestart()
{
    restart = true;
}

void ThreadLoadMagnifierCache::setPage(PageBase *page, double width, double height)
{
    m_page = page;
    m_width = width;
    m_height = height;
}

void ThreadLoadMagnifierCache::run()
{
    if (!m_page)
        return;
    restart = true;
    while (restart) {
        restart = false;
        if (m_width > 0 && m_height > 0) {
            m_page->loadMagnifierPixmapCache(m_width, m_height);
        }
    }
}

PageBase::PageBase(DWidget *parent)
    : DLabel(parent)
{
    setMouseTracking(true);
    setAlignment(Qt::AlignCenter);
}
