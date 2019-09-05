#ifndef MainWindow_H
#define MainWindow_H

#include <DMainWindow>
#include <QVBoxLayout>

#include "mainShow/TitleWidget.h"
#include "mainShow/MainWidget.h"
#include "subjectObserver/MsgSubject.h"


DWIDGET_USE_NAMESPACE

/**
 * @brief The MainWindow class
 * @brief 不做任何的业务处理， 只发送信号， 对应的模块处理相应的业务
 */


class MainWindow : public DMainWindow
{
    Q_OBJECT
public:
    MainWindow(DMainWindow *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *ev) override;

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
    void sendMsg(const int &, const QString &msgContent = "");

private:
    MsgSubject  *m_pMsgSubject = nullptr;

    MainWidget  *m_centralWidget = nullptr;
    TitleWidget *m_titleWidget = nullptr;

    QVBoxLayout *m_centralLayout = nullptr;
    DMenu       *m_menu = nullptr;
};

#endif // MainWindow_H
