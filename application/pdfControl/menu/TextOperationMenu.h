#ifndef TEXTOPERATIONMENU_H
#define TEXTOPERATIONMENU_H

#include "CustomControl/CustomMenu.h"
#include "ModuleHeader.h"
#include <QPointer>

class ColorWidgetAction;

/**
 * @brief The DefaultOperationWidget class
 * @brief   右键  文本菜单操作， 复制、高亮显示、移除高亮显示、添加注释、添加书签
 */
class DocSheet;
class TextOperationMenu : public CustomMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(TextOperationMenu)

public:
    explicit TextOperationMenu(DWidget *parent = nullptr);

public:
    void execMenu(DocSheet *sheet, const QPoint &, const bool &bHigh, const QString &sSelectText, const QString &sUuid);

    void setClickPoint(const QPoint &);

    void setClickPage(int nClickPage);

    void setPStartPoint(const QPoint &pStartPoint);

    void setPEndPoint(const QPoint &pEndPoint);

    void setRemoveEnabled(bool enable);

    inline void setType(Note_Type type)
    {
        m_nType = type;
    }

    // CustomMenu interface
protected:
    void initActions() override;

private:
    QAction *createAction(const QString &, const char *member);
    void notifyMsgToFrame(const int &, const QString &msgContent = "");

private slots:
    void slotSetHighLight(const int &);
    void slotCopyClicked();
    void slotRemoveHighLightClicked();
    void slotAddNoteClicked();
    void slotAddBookMarkClicked();

private:
    int         m_pLightColor = 0;
    QString     m_strSelectText = "";       //  选中的文字
    QString     m_strNoteUuid = "";         //  高亮的uuid

    QAction     *m_pCopy = nullptr;
    QAction     *m_pRemoveHighLight = nullptr;
    QAction     *m_pAddNote = nullptr;
    QAction     *m_pAddBookMark = nullptr;

    ColorWidgetAction   *m_pColorWidgetAction = nullptr;

    QPoint      m_pClickPoint;
    QPoint      m_pStartPoint;          //  右键菜单的 起始点
    QPoint      m_pEndPoint;            //  右键菜单的 结束点
    int         m_nClickPage = -1;
    Note_Type   m_nType = NOTE_HIGHLIGHT;
    QPointer<DocSheet> m_sheet;
};

#endif // TEXTOPERATIONMENU_H
