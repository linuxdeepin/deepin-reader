#ifndef WINDOW_H
#define WINDOW_H

#include <DMainWindow>
#include <QVBoxLayout>

#include "widgets/TitleWidget.h"
#include "widgets/MainWidget.h"
#include "header/MsgSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The Window class
 * @brief 不做任何的业务处理， 只发送信号， 对应的模块处理相应的业务
 */


class Window : public DMainWindow, public IMsgObserver
{
    Q_OBJECT
public:
    Window(DMainWindow *parent = nullptr);
    ~Window() override;

private:
    void initUI();
    void initConnections();
    void initTitlebar();
    void createAction(const QString &actionName, const QString &objectName, const QString &iconName, const char *member);

private slots:
    void action_OpenFile();
    void action_SaveFile();
    void action_SaveAsFile();
    void action_OpenFolder();
    void action_Print();
    void action_Attr();

    void action_Find();
    void action_FullScreen();
    void action_Screening();
    void action_Larger();
    void action_Smaller();
    void action_SwitchTheme();

    void action_Help();

private:
    MsgSubject  *m_pMsgSubject = nullptr;

    MainWidget  *m_centralWidget = nullptr;
    TitleWidget *m_titleWidget = nullptr;

    QVBoxLayout *m_centralLayout = nullptr;
    DMenu       *m_menu = nullptr;

    // IObserver interface
public:
    int update(const int &, const QString &) override;
};

#endif // WINDOW_H
