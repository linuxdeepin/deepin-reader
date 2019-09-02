#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <DWidget>

#include <DListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

#include "NotesItemWidget.h"

#include "header/CustomWidget.h"
#include "header/MsgHeader.h"


DWIDGET_USE_NAMESPACE

class NotesWidget : public CustomWidget
{
    Q_OBJECT

public:
    NotesWidget(CustomWidget *parent = nullptr);

protected:
    void initWidget() override;

private:
    //void initWidget();

private:
    DListWidget *m_pNotesList = nullptr;
    QVBoxLayout *m_pVLayout = nullptr;

    MsgSubject    *m_pMsgSubject = nullptr;

public:
    // IObserver interface
    int update(const int &, const QString &) override;
};

#endif // NOTESFORM_H
