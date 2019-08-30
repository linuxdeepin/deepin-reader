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

#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

class NotesItemWidget : public DWidget, public IThemeObserver
{
    Q_OBJECT

public:
    NotesItemWidget(DWidget *parent = nullptr);
    ~NotesItemWidget() override;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onDltNoteItem();
    void onCopyContant();
    void onShowContextMenu(const QPoint&);

private:
    void initWidget();

public:
    void setPicture(const QString&);
    void setPage(const QString &);
    void setNoteText(const QString &);
    // IObserver interface
    int update(const QString &) override;

private:
    DLabel * m_pPicture = nullptr;
    DLabel * m_pPage = nullptr;
    DTextEdit * m_pTextEdit = nullptr;
    QHBoxLayout * m_pHLayout = nullptr;

    ThemeSubject    *m_pThemeSubject  = nullptr;
};

#endif // NOTESITEMWIDGET_H
