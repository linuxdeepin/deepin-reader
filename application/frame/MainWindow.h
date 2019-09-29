#ifndef MainWindow_H
#define MainWindow_H

#include "application.h"
#include <DMainWindow>
#include <DMenu>

#include "controller/MsgSubject.h"
#include "controller/NotifySubject.h"
#include <QCloseEvent>

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainWindow class
 * @brief 不做任何的业务处理， 只发送信号， 对应的模块处理相应的业务
 */

class MainWindow : public DMainWindow, public IObserver
{
    Q_OBJECT
public:
    MainWindow(DMainWindow *parent = nullptr);
    ~MainWindow() Q_DECL_OVERRIDE;

signals:
    void sigAppExit();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    void initUI();
    void initConnections();
    void initTitlebar();
    QAction *createAction(DMenu *menu, const QString &actionName);

    void openFileOk();

    void onOpenFile();
    void onOpenFolder();
    void onFullScreen();
    void onScreening();


private slots:
    void SlotActionTrigger(const QString &);
    void SlotAppExit();

private:
    void sendMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;

private:
    MsgSubject      *m_pMsgSubject = nullptr;
    NotifySubject   *m_pNotifySubject = nullptr;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

private:
    void setObserverName() Q_DECL_OVERRIDE;
};

#endif // MainWindow_H
