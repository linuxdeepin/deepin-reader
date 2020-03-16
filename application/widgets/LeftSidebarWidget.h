#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H

#include "CustomControl/CustomWidget.h"

/**
 * @brief The LeftShowWidget class
 * @brief   侧边栏显示
 */

class MainOperationWidget;
class DataStackedWidget;

class LeftSidebarWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(LeftSidebarWidget)

public:
    explicit LeftSidebarWidget(DWidget *parent = nullptr);
    ~LeftSidebarWidget() override;

signals:
    void sigAnntationMsg(const int &, const QString &);
    void sigBookMarkMsg(const int &, const QString &);
    void sigDeleteAnntation(const int &, const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    void SetFindOperation(const int &);

    // CustomWidget interface
protected:
    void initWidget() override;

private slots:
    void slotUpdateTheme();

private:
    void initConnections();
    void onSetWidgetVisible(const int &);

    void SlotOpenFileOk(const QString &);

private:
    MainOperationWidget     *m_pMainOperationWidget = nullptr;
    DataStackedWidget       *m_pStackedWidget = nullptr;

    int         m_nSearch = -1;
    bool        m_bOldVisible = false;
    QString     m_strBindPath = "";
};

#endif  // LEFTSHOWWIDGET_H
