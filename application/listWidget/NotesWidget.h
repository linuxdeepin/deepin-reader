#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <DWidget>
#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

class NotesWidget : public DWidget, public IThemeObserver
{
    Q_OBJECT

public:
    NotesWidget(DWidget *parent = nullptr);
    ~NotesWidget() override;

private:
    ThemeSubject    *m_pThemeSubject = nullptr;

    // IObserver interface
public:
    int update(const QString &) override;
};

#endif // NOTESFORM_H
