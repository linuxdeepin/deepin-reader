#ifndef TEXTOPERATIONMENU_H
#define TEXTOPERATIONMENU_H


#include <DMenu>
#include <QAction>
#include "ColorWidgetAction.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The DefaultOperationWidget class
 * @brief   右键  文本菜单操作， 复制、高亮显示、移除高亮显示、添加注释、添加书签
 */

class TextOperationMenu : public DMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(TextOperationMenu)

public:
    explicit TextOperationMenu(DWidget *parent = nullptr);

public:
    void execMenu(const QPoint &, const bool &bHigh, const QString &sSelectText, const QString &sUuid);
    void setClickPoint(const QPoint &);

    void setClickPage(int nClickPage);

private:
    void initMenu();
    QAction *createAction(const QString &, const char *member);
    void sendMsgToFrame(const int &, const QString &msgContent = "");
    void notifyMsgToFrame(const int &, const QString &msgContent = "");

private slots:
    void slotSetHighLight(const int &);

    void slotCopyClicked();
    void slotRemoveHighLightClicked();
    void slotAddNoteClicked();
    void slotAddBookMarkClicked();
    void slotExitFullScreenClicked();

private:
    int         m_pLightColor = 0;
    QString     m_strSelectText = "";       //  选中的文字
    QString     m_strNoteUuid = "";         //  高亮的uuid

    QAction     *m_pCopy = nullptr;
    QAction     *m_pRemoveHighLight = nullptr;
    QAction     *m_pAddBookMark = nullptr;
    QAction     *m_pExitFullScreen = nullptr;

    ColorWidgetAction   *m_pColorWidgetAction = nullptr;

    QPoint      m_pClickPoint;
    int         m_nClickPage = -1;
};

#endif // TEXTOPERATIONMENU_H
