#include "BookMarkWidget.h"

BookMarkWidget::BookMarkWidget(DWidget *parent) :
    DWidget(parent)
{
    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }
}

BookMarkWidget::~BookMarkWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

int BookMarkWidget::update(const QString &)
{
    return 0;
}
