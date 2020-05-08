#ifndef MainWindow_H
#define MainWindow_H

#include <DMainWindow>

class QSignalMapper;
class Central;
class DocSheet;
class MainWindow : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)

public:
    explicit MainWindow(DMainWindow *parent = nullptr);

    ~MainWindow() override;

    void addSheet(DocSheet *sheet);

    bool hasSheet(DocSheet *sheet);

    void activateSheet(DocSheet *sheet);  //将该sheet显示到最前面

public:
    void openfiles(const QStringList &filepaths);

    void doOpenFile(const QString &filePath);

    void setSreenRect(const QRect &); //得到屏幕的分辨率

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void initUI();

    void displayShortcuts();

    void showDefaultSize();

    void initShortCut();

private slots:
    void onShortCut(const QString &);

    void onShowState(int state);

private:
    Qt::WindowStates    m_nOldState = Qt::WindowNoState;        //  旧的窗口状态
    Central            *m_central = nullptr;

public:
    static MainWindow *windowContainSheet(DocSheet *sheet);
    static MainWindow *createWindow();
    static QList<MainWindow *> m_list;
};

#endif // MainWindow_H
