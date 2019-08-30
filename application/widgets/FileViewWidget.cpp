#include "FileViewWidget.h"

FileViewWidget::FileViewWidget(DWidget *parent) :
    DWidget (parent)
{
    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }
}

FileViewWidget::~FileViewWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

int FileViewWidget::update(const QString &)
{
    return 0;
}
