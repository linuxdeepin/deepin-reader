#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <DWidget>
#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

class NotesForm : public DWidget, public IThemeObserver
{
    Q_OBJECT

public:
    NotesForm(DWidget *parent = nullptr);
    ~NotesForm();

public:
    // IObserver interface
    int update(const QString &);

private:
    ThemeSubject    *m_pThemeSubject = nullptr;

};

#endif // NOTESFORM_H
