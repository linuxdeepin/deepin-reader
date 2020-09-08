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
#ifndef CENTRAL_H
#define CENTRAL_H

#include "widgets/CustomWidget.h"

class CentralDocPage;
class CentralNavPage;
class TitleMenu;
class TitleWidget;
class DocSheet;
class QStackedLayout;

/**
 * @brief The CustomWidget class
 * 嵌入mainwindow的中心控件
 */
class Central : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(Central)

public:
    explicit Central(QWidget *parent = nullptr);

    ~Central() override;

    /**
     * @brief openFilesExec
     * 阻塞式打开文件选择对话框选择文件并打开
     * @return
     */
    void openFilesExec();

    /**
     * @brief openFiles
     * 批量打开文件 会进行是否已存在判断
     * @param filePathList 文件路径列表
     */
    void openFiles(QStringList filePathList);

    /**
     * @brief doOpenFile
     * 直接打开文件 不进行判断
     * @param filePath 文件路径
     */
    void doOpenFile(QString filePath);

    /**
     * @brief zoomIn
     * 文档放大
     */
    void zoomIn();

    /**
     * @brief zoomOut
     * 文档缩小
     */
    void zoomOut();

    /**
     * @brief addSheet
     * 直接添加一个文档页
     * @param sheet
     */
    void addSheet(DocSheet *sheet);

    /**
     * @brief hasSheet
     * 本窗口是否含有该文档页
     * @param sheet
     * @return
     */
    bool hasSheet(DocSheet *sheet);

    /**
     * @brief showSheet
     * 显示传入的文档页
     * @param sheet
     */
    void showSheet(DocSheet *sheet);

    /**
     * @brief saveAll
     * 保存本窗口所有文档页
     * @return 保存成功
     */
    bool saveAll();

    /**
     * @brief handleShortcut
     * 本窗口处理快捷键
     * @param shortcut 快捷键字符串
     */
    void handleShortcut(QString shortcut);

    /**
     * @brief titleWidget
     * 获取提供给mainwindow的标题控件
     * @return
     */
    TitleWidget *titleWidget();

    /**
     * @brief docPage
     * 获取文档浏览堆栈窗口
     * @return
     */
    CentralDocPage *docPage();

    /**
     * @brief setMenu
     * 设置菜单
     * @return
     */
    void setMenu(TitleMenu *menu);

signals:
    /**
     * @brief sigNeedClose
     * 需要关闭本窗口
     */
    void sigNeedClose();

public slots:
    /**
     * @brief onSheetCountChanged
     * 根据当前文档个数变化后改变显示导航页面还是文档浏览页面
     * @param count 当前文档个数
     */
    void onSheetCountChanged(int count);

    /**
     * @brief onMenuTriggered
     * 菜单功能触发处理
     * @param action 菜单功能名称
     */
    void onMenuTriggered(const QString &action);

    /**
     * @brief onOpenFilesExec
     * 打开文件请求处理
     */
    void onOpenFilesExec();

    /**
     * @brief onNeedActivateWindow
     * 将窗口设为活跃窗口的请求处理
     */
    void onNeedActivateWindow();

    /**
     * @brief onShowTips
     * 弹出提示的请求处理
     * @param text 请求显示文本
     * @param iconIndex 请求显示图标索引
     */
    void onShowTips(const QString &text, int iconIndex = 0);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;

    void dropEvent(QDropEvent *event) override;

private:
    QStackedLayout      *m_layout = nullptr;
    CentralDocPage      *m_docPage = nullptr;
    CentralNavPage      *m_navPage = nullptr;
    TitleMenu           *m_menu = nullptr;
    TitleWidget         *m_widget = nullptr;
};

#endif  // CENTRAL_H
