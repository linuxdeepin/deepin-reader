#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H

#include "subjectObserver/CustomWidget.h"

/**
 * @brief The LeftShowWidget class
 * @brief   侧边栏显示
 */

class LeftSidebarWidget : public CustomWidget
{
    Q_OBJECT
public:
    LeftSidebarWidget(CustomWidget *parent = nullptr);

signals:
    void sigStackSetCurIndex(const int &);
    void sigWidgetVisible(const int &);

private slots:
    void slotStackSetCurIndex(const int &);
    void slotWidgetVisible(const int &);

private:
    void initConnections();

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // LEFTSHOWWIDGET_H
