#ifndef NOTESFORM_H
#define NOTESFORM_H

#include <DListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

#include "NotesItemWidget.h"

#include "header/CustomWidget.h"



class NotesWidget : public CustomWidget
{
    Q_OBJECT

public:
    NotesWidget(CustomWidget *parent = nullptr);

protected:
    void initWidget() override;

private:
    DListWidget *m_pNotesList = nullptr;
    QVBoxLayout *m_pVLayout = nullptr;

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;
};

#endif // NOTESFORM_H
