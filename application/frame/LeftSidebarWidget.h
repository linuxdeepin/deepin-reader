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

signals:
    void sigStackSetCurIndex(const int &);
    void sigWidgetVisible(const int &);
    void sigJumpToPrevPage(const QString &);
    void sigJumpToNextPage(const QString &);

    void sigJumpToPrevPage(const int &, const QString &);
    void sigJumpToNextPage(const int &, const QString &);

private slots:
    void slotStackSetCurIndex(const int &);
    void slotWidgetVisible(const int &);
    void slotUpdateTheme();


    void slotJumpToPrevPage(const QString &);
    void slotJumpToNextPage(const QString &);

private:
    void initConnections();

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
};

#endif  // LEFTSHOWWIDGET_H
