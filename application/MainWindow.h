#ifndef MainWindow_H
#define MainWindow_H

#include <DMainWindow>
#include "IObserver.h"

DWIDGET_USE_NAMESPACE

class QSignalMapper;
class Central;
class MainWindow : public DMainWindow, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)

public:
    explicit MainWindow(DMainWindow *parent = nullptr);

    ~MainWindow() override;

private:
    static MainWindow *g_onlyMainWindow;

public:
    static MainWindow *Instance();

    void openfile(const QString &filepath);

    void setSreenRect(const QRect &); //得到屏幕的分辨率

    void SetSliderShowState(const int &);

public:
    int dealWithData(const int &, const QString &) override;

protected:
    void showEvent(QShowEvent *ev) override;

    void closeEvent(QCloseEvent *event) override;

private:
    void initUI();

    void onAppExit();

    void initThemeChanged();

    void setCurTheme();

    void displayShortcuts();

    void notifyMsg(const int &, const QString &msgContent = "") override;

    void showDefaultSize();

    void initShortCut();

private slots:
    void slotShortCut(const QString &);

private:
    QList<int>          m_pMsgList;
    Qt::WindowStates    m_nOldState = Qt::WindowNoState;        //  旧的窗口状态
    Central            *m_central = nullptr;
};

#endif // MainWindow_H
