/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* Central(NaviPage ViewPage)
*
* CentralNavPage(openfile)
*
* CentralDocPage(DocTabbar DocSheets)
*
* DocSheet(SheetSidebar SheetBrowser document)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MainWindow_H
#define MainWindow_H

#include <DMainWindow>

class Central;
class DocSheet;
class TitleMenu;
class QPropertyAnimation;
class MainWindow : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
    Q_PROPERTY(QList<QWidget *> orderWidgets WRITE updateOrderWidgets)

public:
    /**
     * @brief MainWindow
     * 根据文件路径列表会直接打开对应文件
     * @param filePathList 文件路径列表
     * @param parent
     */
    explicit MainWindow(QStringList filePathList, DMainWindow *parent = nullptr);

    /**
     * @brief MainWindow
     * 直接显示传入已经存在的sheet
     * @param sheet
     * @param parent
     */
    explicit MainWindow(DocSheet *sheet, DMainWindow *parent = nullptr);

    ~MainWindow() override;

    /**
     * @brief addSheet
     * 添加新文档窗口
     * @param sheet
     */
    void addSheet(DocSheet *sheet);

    /**
     * @brief hasSheet
     * 是否有这个文档
     * @param sheet
     * @return
     */
    bool hasSheet(DocSheet *sheet);

    /**
     * @brief activateSheet
     * 活动文档显示到最前面
     * @param sheet 哪个文档
     */
    void activateSheet(DocSheet *sheet);

    /**
     * @brief closeWithoutSave
     * 不需要保存的子窗口直接关闭
     */
    void closeWithoutSave();

    /**
     * @brief setDocTabBarWidget
     * 全屏时设置tabbar
     * @param widget tabbar
     */
    void setDocTabBarWidget(QWidget *widget);

    /**
     * @brief 调整标题控件大小
     */
    void resizeFullTitleWidget();

    /**
     * @brief 更新TAB控件顺序
     * @param orderlst
     */
    void updateOrderWidgets(const QList<QWidget *> &orderlst);

public:
    /**
     * @brief openfiles
     * 打开多个文件
     * @param filepaths 多个文件路径
     */
    void openfiles(const QStringList &filepaths);

    /**
     * @brief doOpenFile
     * 打开单个文件
     * @param filePath 文件全路径
     */
    void doOpenFile(const QString &filePath);

protected:
    void closeEvent(QCloseEvent *event) override;

    bool eventFilter(QObject *obj, QEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

private:
    /**
     * @brief initBase
     * 初始化基础项
     */
    void initBase();

    /**
     * @brief initUI
     * 初始化UI
     */
    void initUI();

    /**
     * @brief initShortCut
     * 初始化响应快捷键接口
     */
    void initShortCut();

    /**
     * @brief showDefaultSize
     * 显示默认大小
     */
    void showDefaultSize();

    /**
     * @brief zoomIn
     * 放大
     */
    void zoomIn();

    /**
     * @brief zoomOut
     * 缩小
     */
    void zoomOut();

private slots:
    /**
     * @brief onDelayInit
     * 延时初始化
     */
    void onDelayInit();

    /**
     * @brief onShortCut
     * 处理快捷键事件
     */
    void onShortCut(const QString &);

    /**
     * @brief onUpdateTitleLabelRect
     * 更新提示标签大小
     */
    void onUpdateTitleLabelRect();

    /**
     * @brief Application::onTouchPadEventSignal
     * 处理触控板手势信号
     * @param name 触控板事件类型(手势或者触摸类型) pinch 捏 tap 敲 swipe 滑 右键单击 单键
     * @param direction 手势方向 触控板上 up 触控板下 down 左 left 右 right 无 none 向内 in 向外 out  触控屏上 top 触摸屏下 bot
     * @param fingers 手指数量 (1,2,3,4,5)
     * 注意libinput接收到触摸板事件后将接收到的数据通过Event广播出去
     */
    void onTouchPadEventSignal(QString name, QString direction, int fingers);

    /**
     * @brief 全屏
     */
    void onMainWindowFull();

    /**
     * @brief 退出全屏
     */
    void onMainWindowExitFull();

    /**
     * @brief 标题悬浮动画结束
     */
    void onTitleAniFinished();

private:
    QWidget *m_FullTitleWidget = nullptr;
    QWidget *m_docTabWidget = nullptr;
    QPropertyAnimation *m_TitleAnimation = nullptr;

    TitleMenu *m_menu = nullptr;

    Central *m_central = nullptr;

    bool m_needSave = true;
    int m_lastWindowState = Qt::WindowNoState;

    QStringList m_initFilePathList;

public:
    static MainWindow *windowContainSheet(DocSheet *sheet);
    static bool allowCreateWindow();
    static MainWindow *createWindow(QStringList filePathList = QStringList());
    static MainWindow *createWindow(DocSheet *sheet);
    static QList<MainWindow *> m_list;

    QTimer *m_showMenuTimer = nullptr;
};

#endif // MainWindow_H
