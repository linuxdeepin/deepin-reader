#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H

#include "CustomControl/CustomWidget.h"

/**
 * @brief The LeftShowWidget class
 * @brief   侧边栏显示
 */

class LeftSidebarWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(LeftSidebarWidget)

public:
    explicit LeftSidebarWidget(CustomWidget *parent = nullptr);
    ~LeftSidebarWidget() Q_DECL_OVERRIDE;

signals:
    void sigSearchWidgetState(const int &);
    void sigOpenFileOk(const QString &);
    void sigTitleMsg(const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

private slots:
    void SlotOpenFileOk(const QString &);

    void slotUpdateTheme();
    void slotSetStackCurIndex(const int &);
    void slotTitleMsg(const QString &);

private:
    void initConnections();

    void __DeleteItemByKey();
    void __DealWithPressKey(const QString &);
    void onSetWidgetVisible(const int &);

    void onJumpToPrevPage();
    void onJumpToNextPage();

    void onDocProxyMsg(const QString &);

};

#endif  // LEFTSHOWWIDGET_H
