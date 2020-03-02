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
    ~LeftSidebarWidget() override;

signals:
    void sigSearchWidgetState(const int &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    int qDealWithData(const int &, const QString &) override;

    // CustomWidget interface
protected:
    void initWidget() override;

private slots:

    void slotUpdateTheme();

private:
    void initConnections();
    void onSetWidgetVisible(const int &);

    void SlotOpenFileOk(const QString &);
};

#endif  // LEFTSHOWWIDGET_H
