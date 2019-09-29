#ifndef NOTESITEMWIDGET_H
#define NOTESITEMWIDGET_H

#include <DLabel>
#include <DTextEdit>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include "translator/Frame.h"
#include "CustomItemWidget.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   注释和搜索item
 */


class NotesItemWidget : public CustomItemWidget
{
    Q_OBJECT

public:
    NotesItemWidget(CustomItemWidget *parent = nullptr);

public:
    void setTextEditText(const QString &);
    void setSerchResultText(const QString &);

    inline void setNoteUUid(const QString &uuid)
    {
        m_strUUid = uuid;
    }

private slots:
    void slotDltNoteItem();
    void slotCopyContant();
    void slotShowContextMenu(const QPoint &);

protected:
    void initWidget() Q_DECL_OVERRIDE;

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

private:
    DLabel *m_pSearchResultNum = nullptr;
    DTextEdit *m_pTextEdit = nullptr;

    QString m_strUUid;    // 当前注释唯一标识
};

#endif // NOTESITEMWIDGET_H
