#ifndef MainWindow_H
#define MainWindow_H

#include "application.h"
#include <DMainWindow>
#include <DMenu>

#include "controller/MsgSubject.h"
#include "controller/NotifySubject.h"

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
    ~MainWindow() override;

private:
    void initUI();
    void initConnections();
    void initTitlebar();
    QAction *createAction(DMenu *menu, const QString &actionName, const char *member, const bool &disable = true);

    void openFileOk();

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

private:
    void sendMsg(const int &, const QString &msgContent = "") override;

private:
    MsgSubject      *m_pMsgSubject = nullptr;
    NotifySubject   *m_pNotifySubject = nullptr;
    DMenu           *m_menu = nullptr;

private:
    QAction *m_pSaveFile = nullptr;
    QAction *m_pSaveAsFile = nullptr;
    QAction *m_pOpenFolder = nullptr;
    QAction *m_pFilePrint = nullptr;
    QAction *m_pFileAttr = nullptr;
    QAction *m_pFileFind = nullptr;
    QAction *m_pFileFullScreen = nullptr;
    QAction *m_pFileScreening = nullptr;
    QAction *m_pFileLarger = nullptr;
    QAction *m_pFileSmaller = nullptr;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

private:
    void setObserverName(const QString &name) override;
};

#endif // MainWindow_H
