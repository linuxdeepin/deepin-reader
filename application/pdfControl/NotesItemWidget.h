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

#include "CustomItemWidget.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   注释和搜索item
 */

class NotesItemWidget : public CustomItemWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(NotesItemWidget)

public:
    explicit NotesItemWidget(CustomItemWidget *parent = nullptr);

public:
    void setTextEditText(const QString &);
    void setSerchResultText(const QString &);
    inline void setNoteSigne(bool note)
    {
        m_isNote = note;
    }

    inline void setNoteUUid(const QString &uuid)
    {
        m_strUUid = uuid;
    }
    inline QString noteUUId() const
    {
        return m_strUUid;
    }
    inline QString note() const
    {
        return m_strNote;
    }

    inline void setNote(const QString &note)
    {
        m_strNote = note;
    }

    void setBSelect(const bool &paint);

private slots:
    void slotDltNoteContant();
    void slotCopyContant();
    void slotShowContextMenu(const QPoint &);
    void slotUpdateTheme();

protected:
    void initWidget() Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

private:
    QString calcText(const QFont &font, const QString &note, const QSize &size/*const int MaxWidth*/);

private:
    DLabel *m_pSearchResultNum = nullptr;
    DLabel *m_pTextLab = nullptr;
    QString m_strUUid;    // 当前注释唯一标识
    bool m_isNote = true;// 是否是注释窗体,如果不是则不显示右键菜单
    QString m_strNote;   // 注释内容
    bool m_bPaint = false;
    DMenu *m_menu = nullptr;
};

#endif // NOTESITEMWIDGET_H
