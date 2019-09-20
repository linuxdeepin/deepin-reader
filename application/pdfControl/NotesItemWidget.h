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
#include "subjectObserver/CustomWidget.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   注释和搜索item
 */


class NotesItemWidget : public CustomWidget
{
    Q_OBJECT

public:
    NotesItemWidget(CustomWidget *parent = nullptr);

public:
    void setLabelImage(const QImage &);
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
    DLabel *m_pPicture = nullptr;
    DLabel *m_pPage = nullptr;
    DLabel *m_pSearchResultNum = nullptr;
    DTextEdit *m_pTextEdit = nullptr;
    QHBoxLayout *m_pHLayout = nullptr;
    int m_page = -1;
};

#endif // NOTESITEMWIDGET_H
