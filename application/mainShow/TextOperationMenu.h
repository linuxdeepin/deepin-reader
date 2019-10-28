#ifndef TEXTOPERATIONMENU_H
#define TEXTOPERATIONMENU_H


#include <DMenu>
#include <QAction>

DWIDGET_USE_NAMESPACE

/**
 * @brief The DefaultOperationWidget class
 * @brief   右键  文本菜单操作， 复制、高亮显示、移除高亮显示、添加注释、添加书签
 */

class TextOperationMenu : public DMenu
{
    Q_OBJECT
public:
    TextOperationMenu(DWidget *parent = nullptr);

public:
    void execMenu(const QPoint &, const bool &bHigh, const QString &sSelectText, const QString &sUuid);

private:
    void initMenu();
    QAction *createAction(const QString &, const char *member);
    void sendMsgToFrame(const int &, const QString &msgContent = "");

private slots:
    void slotSetHighLight(int);

    void slotCopyClicked();
    void slotAddHighLightClicked();
    void slotRemoveHighLightClicked();
    void slotAddNoteClicked();
    void slotAddBookMarkClicked();

private:
    int         m_pLightColor = -1;
    int         m_nShowY = -1;
    QString     m_strSelectText = "";       //  选中的文字
    QString     m_strNoteUuid = "";   //  高亮的uuid

    QAction     *m_pRemoveHighLight = nullptr;
    QAction     *m_pAddBookMark = nullptr;
};

#endif // TEXTOPERATIONMENU_H
