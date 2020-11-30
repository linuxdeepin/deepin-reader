/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H

#include "BaseWidget.h"

#include <QResizeEvent>
#include <DToolButton>
#include <QStackedLayout>

enum PreviewWidgesFlag {
    PREVIEW_THUMBNAIL = 0x0001,
    PREVIEW_CATALOG   = 0x0002,
    PREVIEW_BOOKMARK  = 0x0004,
    PREVIEW_NOTE      = 0x0008,
    PREVIEW_SEARCH    = 0x0010
};
Q_DECLARE_FLAGS(PreviewWidgesFlags, PreviewWidgesFlag);
Q_DECLARE_OPERATORS_FOR_FLAGS(PreviewWidgesFlags)

namespace deepin_reader {
struct SearchResult;
class Annotation;
}

class DocSheet;
class ThumbnailWidget;
class CatalogWidget;
class BookMarkWidget;
class NotesWidget;
class SearchResWidget;
class QButtonGroup;

/**
 * @brief The SheetSidebar class
 * 左侧栏控件
 */
class SheetSidebar : public BaseWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SheetSidebar)
    Q_PROPERTY(QPoint movepos READ pos WRITE move)

public :
    explicit SheetSidebar(DocSheet *parent = nullptr, PreviewWidgesFlags widgesFlag = PREVIEW_THUMBNAIL | PREVIEW_CATALOG | PREVIEW_BOOKMARK | PREVIEW_NOTE);
    ~SheetSidebar() override;

    /**
     * @brief setBookMark
     * 设置书签状态
     * @param index
     * @param state
     */
    void setBookMark(int index, int state);

    /**
     * @brief setCurrentPage
     * 设置当前页
     * @param page
     */
    void setCurrentPage(int page);

    /**
     * @brief handleOpenSuccess
     * 文档打开成功
     */
    void handleOpenSuccess();

    /**
     * @brief handleFindOperation
     * 搜索状态
     */
    void handleFindOperation(int, const QString &strFind = "");

    /**
     * @brief handleFindContentComming
     * 搜索节点处理
     */
    void handleFindContentComming(const deepin_reader::SearchResult &);

    /**
     * @brief handleFindFinished
     * 搜索结束
     * @return
     */
    int  handleFindFinished();

    /**
     * @brief handleRotate
     * 文档旋转
     */
    void handleRotate();

    /**
     * @brief changeResetModelData
     * 重置模型数据
     */
    void changeResetModelData();

public slots:
    /**
     * @brief handleUpdateThumbnail
     * 刷新指定页码数据
     * @param index
     */
    void handleUpdateThumbnail(const int &index);

    /**
     * @brief handleUpdatePartThumbnail
     * 缓存中无缩略图数据,进行刷新
     * @param index
     */
    void handleUpdatePartThumbnail(const int &index);

    /**
     * @brief handleAnntationMsg
     * 注释操作类型处理
     * @param anno
     */
    void handleAnntationMsg(const int &, int, deepin_reader::Annotation *anno);

    // QObject interface
public:
    /**
    * @brief event
    * 事件处理
    * @param event
    * @return
    */
    bool event(QEvent *event) Q_DECL_OVERRIDE;

protected:
    /**
     * @brief initWidget
     * 初始化控件
     */
    void initWidget();

    /**
     * @brief adaptWindowSize
     * 窗口大小变化
     */
    void adaptWindowSize(const double &) Q_DECL_OVERRIDE;

protected:
    /**
     * @brief resizeEvent
     * 窗口大小变化事件
     * @param event
     */
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief keyPressEvent
     * 键盘按键事件
     * @param event
     */
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief showMenu
     * 菜单显示
     */
    void showMenu();

private slots:
    /**
     * @brief onBtnClicked
     * 缩略图,目录,书签,注释分页按钮切换
     * @param index
     */
    void onBtnClicked(int index);

    /**
     * @brief onUpdateWidgetTheme
     * 主题变化刷新
     */
    void onUpdateWidgetTheme();

    /**
     * @brief onHandWidgetDocOpenSuccess
     * 文档打开成功处理
     */
    void onHandWidgetDocOpenSuccess();

    /**
     * @brief onHandleOpenSuccessDelay
     * 文档打开成功延迟处理
     */
    void onHandleOpenSuccessDelay();

private:
    /**
     * @brief createBtn
     * 分页按钮创建
     * @param btnName
     * @param objName
     * @return
     */
    DToolButton *createBtn(const QString &btnName, const QString &objName);

    /**
     * @brief dealWithPressKey
     * 键盘按键处理
     * @param sKey
     */
    void dealWithPressKey(const QString &sKey);

    /**
     * @brief onJumpToPrevPage
     * 上一页跳转
     */
    void onJumpToPrevPage();

    /**
     * @brief onJumpToPageUp
     * 上一翻页
     */
    void onJumpToPageUp();

    /**
     * @brief onJumpToNextPage
     * 下一页跳转
     */
    void onJumpToNextPage();

    /**
     * @brief onJumpToPageDown
     * 下一翻页
     */
    void onJumpToPageDown();

    /**
     * @brief deleteItemByKey
     * 删除按键处理
     */
    void deleteItemByKey();

private:
    qreal m_scale = -1;
    int  m_searchId = -1;
    bool m_bOldVisible = false;
    bool m_bOpenDocOpenSuccess = false;

    DocSheet *m_sheet = nullptr;
    QStackedLayout *m_stackLayout = nullptr;
    QMap<int, DToolButton *> m_btnGroupMap;
    PreviewWidgesFlags m_widgetsFlag;

    ThumbnailWidget *m_thumbnailWidget = nullptr;
    CatalogWidget   *m_catalogWidget = nullptr;
    BookMarkWidget  *m_bookmarkWidget = nullptr;
    NotesWidget     *m_notesWidget = nullptr;
    SearchResWidget *m_searchWidget = nullptr;
};
#endif  // LEFTSHOWWIDGET_H
