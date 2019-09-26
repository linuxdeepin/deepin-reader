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
    void setLabelPage(const QString &);
    void setTextEditText(const QString &);
    void setSerchResultText(const QString &);

    inline int page() const
    {
        return m_page;
    }

    inline void setPage(const int &page)
    {
        m_page = page;
    }

    inline void setNoteUUid(const QString &uuid)
    {
        m_strUUid = uuid;
    }

private slots:
    void slotDltNoteItem();
    void slotCopyContant();
    void slotShowContextMenu(const QPoint &);

protected:
    void initWidget() override;

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;

private:
    DLabel *m_pPage = nullptr;
    DLabel *m_pSearchResultNum = nullptr;
    DTextEdit *m_pTextEdit = nullptr;

    int m_page = -1;      // 当前注释所在页
    QString m_strUUid;    // 当前注释唯一标识
};

#endif // NOTESITEMWIDGET_H
