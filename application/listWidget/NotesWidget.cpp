#include "NotesWidget.h"

NotesForm::NotesForm(DWidget *parent) :
    DWidget(parent)
{
    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }
}

NotesForm::~NotesForm()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

int NotesForm::update(const QString &)
{
    return 0;
}
