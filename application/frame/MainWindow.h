#ifndef MainWindow_H
#define MainWindow_H

#include "application.h"
#include <DMainWindow>
#include <DMenu>
#include <QSignalMapper>
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
    Q_DISABLE_COPY(MainWindow)

public:
    explicit MainWindow(DMainWindow *parent = nullptr);
    ~MainWindow() Q_DECL_OVERRIDE;

    void openfile(const QString &filepath);
    void setSreenRect(const QRect &); //得到屏幕的分辨率

signals:
    void sigOpenFileOk();
    void sigAppExit();
    void sigAppShowState(const int &);
    void sigFullScreen();
    void sigSetAppTitle(const QString &);
    void sigSpacePressed();

protected:
    void showEvent(QShowEvent *ev) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    void initUI();
    void initConnections();
    void initTitlebar();
    QAction *createAction(DMenu *menu, const QString &actionName, const QString &);

    void onOpenFile();
    void onOpenFolder();

    void onScreening();

    void initThemeChanged();
    void setCurTheme();

private slots:
    void slotOpenFileOk();
    void slotAppExit();
    void slotFullScreen();
    void slotAppShowState(const int &);
    void slotSetAppTitle(const QString &);

    void slotActionTrigger(const QString &);

private:
    void sendMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;

private:
    MsgSubject      *m_pMsgSubject = nullptr;
    NotifySubject   *m_pNotifySubject = nullptr;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

private:
    void createActionMap(DMenu *m_menu, QSignalMapper *pSigManager, const QStringList &firstActionList, const QStringList &firstActionObjList);
};

#endif // MainWindow_H
