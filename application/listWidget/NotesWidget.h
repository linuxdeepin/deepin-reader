#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <DWidget>

#include <DListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

#include "NotesItemWidget.h"

#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"


DWIDGET_USE_NAMESPACE

class NotesWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT

public:
    NotesWidget(DWidget *parent = nullptr);
    ~NotesWidget() override;

private:
    void initWidget();

private:
    DListWidget * m_pNotesList = nullptr;
    QVBoxLayout * m_pVLayout = nullptr;

    MsgSubject    *m_pMsgSubject = nullptr;

public:
    // IObserver interface
    int update(const int&, const QString &) override;
};

#endif // NOTESFORM_H
