#ifndef MainWindow_H
#define MainWindow_H

#include <DMainWindow>
#include <DMenu>

#include "application.h"

DWIDGET_USE_NAMESPACE

class QSignalMapper;

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
    ~MainWindow() override;

    void openfile(const QString &filepath);
    void setSreenRect(const QRect &); //得到屏幕的分辨率

signals:
    void sigAppShowState(const int &);
    void sigFullScreen();
    void sigSpacePressed();

    void sigDealWithData(const int &, const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    int dealWithData(const QString &) override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *ev) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void initUI();
    void initConnections();

    void onSetAppTitle(const QString &);
    void onAppExit();

    void initThemeChanged();
    void setCurTheme();
    void displayShortcuts();

    // IObserver interface
//    void sendMsg(const int &, const QString &msgContent = "") override;
    void notifyMsg(const int &, const QString &msgContent = "") override;

    void showDefaultSize();

    void initShortCut();

private slots:
    void slotFullScreen();
    void slotAppShowState(const int &);

    void slotDealWithData(const int &, const QString &);

    void slotShortCut(const QString &);
    void SlotSlideShow();

private:
    QList<int>      m_pMsgList;
    Qt::WindowStates    m_nOldState = Qt::WindowNoState;        //  旧的窗口状态


};

#endif // MainWindow_H
