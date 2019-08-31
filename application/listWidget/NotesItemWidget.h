#ifndef NOTESITEMWIDGET_H
#define NOTESITEMWIDGET_H

#include <DWidget>
#include <DLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <DTextEdit>
#include <QContextMenuEvent>

#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"

DWIDGET_USE_NAMESPACE

class NotesItemWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT

public:
    NotesItemWidget(DWidget *parent = nullptr);
    ~NotesItemWidget() override;

public:
    void setLabelPix(const QString&);
    void setLabelPage(const QString&);
    void setTextEditText(const QString&);

private slots:
    void slotDltNoteItem();
    void slotCopyContant();
    void slotShowContextMenu(const QPoint&);

private:
    void initWidget();

public:
    // IObserver interface
    int update(const int&, const QString &) override;

private:
    DLabel * m_pPicture = nullptr;
    DLabel * m_pPage = nullptr;
    DTextEdit * m_pTextEdit = nullptr;
    QHBoxLayout * m_pHLayout = nullptr;

    MsgSubject    *m_pMsgSubject  = nullptr;
};

#endif // NOTESITEMWIDGET_H
