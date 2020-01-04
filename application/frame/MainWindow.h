#ifndef MainWindow_H
#define MainWindow_H

#include "application.h"
#include <DMainWindow>
#include <DMenu>
#include <QSignalMapper>
#include "controller/NotifySubject.h"
#include <QCloseEvent>
#include <QResizeEvent>

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
    void sigAppShowState(const int &);
    void sigFullScreen();
    void sigSpacePressed();

    void sigDealWithData(const int &, const QString &);

    // QWidget interface
protected:
    void showEvent(QShowEvent *ev) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;

private:
    void initUI();
    void initConnections();
    void initTitlebar();

    QAction *createAction(DMenu *menu, const QString &actionName, const QString &);

    void onOpenFile();
    void onOpenFolder();
    void onScreening();
    void onSetAppTitle(const QString &);
    void onAppExit();

    void initThemeChanged();
    void setCurTheme();
    void dealWithKeyEvent(const QString &key);
    void onOpenAppHelp();
    void displayShortcuts();

    void createActionMap(DMenu *m_menu, QSignalMapper *pSigManager, const QStringList &firstActionList, const QStringList &firstActionObjList);

    // IObserver interface
    void sendMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;

    void showDefaultSize();

    void initShortCut();

private slots:
    void slotOpenFileOk();
    void slotFullScreen();
    void slotAppShowState(const int &);

    void slotActionTrigger(const QString &);
    void slotDealWithData(const int &, const QString &);

    void slotShortCut(const QString &);

private:
    SubjectThread   *m_pNotifySubject = nullptr;

    QList<int>      m_pMsgList;
    QStringList     m_pFilterList;
    Qt::WindowStates    m_nOldState = Qt::WindowNoState;        //  旧的窗口状态

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

};

#endif // MainWindow_H
