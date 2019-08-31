#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <DWidget>
#include <DListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

#include "NotesItemWidget.h"
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
    void initWidget();

public:
    // IObserver interface
    int update(const QString &) override;

private:
    DListWidget * m_pNotesList = nullptr;
    QVBoxLayout * m_pVLayout = nullptr;
    ThemeSubject    *m_pThemeSubject = nullptr;
};

#endif // NOTESFORM_H
