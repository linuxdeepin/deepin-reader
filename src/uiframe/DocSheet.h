/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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
#ifndef DocSheet_H
#define DocSheet_H

#include "document/Model.h"

#include <DSplitter>

class SheetSidebar;
class SlideWidget;
class EncryptionPage;
struct SheetOperation {
    Dr::LayoutMode layoutMode   = Dr::SinglePageMode;
    Dr::MouseShape mouseShape   = Dr::MouseShapeNormal;
    Dr::ScaleMode scaleMode = Dr::FitToPageWorHMode;
    Dr::Rotation rotation       = Dr::RotateBy0;
    qreal scaleFactor           = 1.0;
    bool sidebarVisible         = false;
    int  sidebarIndex           = 0;
    int  currentPage            = 1;
};

class SheetBrowser;

/**
 * @brief The DocSheet class
 * 嵌入当前窗体中心控件，一个类表示并显示操作一个文档
 */
class DocSheet : public Dtk::Widget::DSplitter
{
    Q_OBJECT
    Q_DISABLE_COPY(DocSheet)

    friend class SheetSidebar;
    friend class SheetBrowser;
    friend class Database;
public:
    explicit DocSheet(Dr::FileType fileType, QString filePath, QWidget *parent = nullptr);

    ~DocSheet();

public:
    /**
     * @brief firstThumbnail
     * 获取对应文档的第一页
     * @param filePath 需要处理的文档路径
     */
    static QImage firstThumbnail(const QString &filePath);

    /**
     * @brief existFileChanged
     * 进程所有的文档是否存在被修改的
     * @return
     */
    static bool existFileChanged();

    /**
     * @brief getUuid
     * 根据文档返回对应的uuid
     * @param sheet
     * @return
     */
    static QUuid getUuid(DocSheet *sheet);

    /**
     * @brief existSheet
     * 进程是否存在该文档
     * @param sheet
     * @return
     */
    static bool existSheet(DocSheet *sheet);

    /**
     * @brief getSheet
     * 根据uuid返回sheet
     * @param uuid
     * @return
     */
    static DocSheet *getSheet(QString uuid);

    static QMap<QString, DocSheet *> g_map;

public:
    /**
     * @brief openFileExec
     * 阻塞式打开文档
     * @param password 文档密码
     * @return
     */
    bool openFileExec(const QString &password);

    /**
     * @brief pagesNumber
     * 获取文档总页数
     * @return
     */
    int pagesNumber();

    /**
     * @brief currentPage
     * 获取当前文档页数 base 1
     * @return
     */
    int currentPage();

    /**
     * @brief currentPage
     * 获取当前文档索引值 base 0
     * @return
     */
    int currentIndex();

    /**
     * @brief jumpToIndex
     * 跳转到索引
     * @param index 索引值
     */
    void jumpToIndex(int index);

    /**
     * @brief jumpToPage
     * 跳转到页
     * @param page 页码
     */
    void jumpToPage(int page);

    /**
     * @brief jumpToFirstPage
     * 跳转到第一页
     */
    void jumpToFirstPage();

    /**
     * @brief jumpToLastPage
     * 跳转到最后一页
     */
    void jumpToLastPage();

    /**
     * @brief jumpToNextPage
     * 跳转到下一页
     */
    void jumpToNextPage();

    /**
     * @brief jumpToPrevPage
     * 跳转到上一页
     */
    void jumpToPrevPage();

    /**
     * @brief rotateLeft
     * 左旋转
     */
    void rotateLeft();

    /**
     * @brief rotateRight
     * 右旋转
     */
    void rotateRight();

    /**
     * @brief outline
     * 获取跳转链接列表
     * @return
     */
    deepin_reader::Outline outline();

    /**
     * @brief jumpToOutline
     * 跳转到指定位置
     * @param left 距离左边距离
     * @param top 距离上边距离
     * @param page 跳转的页码
     */
    void jumpToOutline(const qreal  &left, const qreal &top, unsigned int page);

    /**
     * @brief jumpToHighLight
     * 跳转到注释位置
     * @param annotation 注释
     * @param index 跳转的索引值
     */
    void jumpToHighLight(deepin_reader::Annotation *annotation, const int index);

    /**
     * @brief setBookMark
     * 设置书签
     * @param index 被设置的索引
     * @param state 被设置的状态
     */
    void setBookMark(int index, int state);

    /**
     * @brief setBookMarks
     * 批量设置书签
     * @param indexlst 被设置的索引列表
     * @param state 被设置的状态
     */
    void setBookMarks(const QList<int> &indexlst, int state);

    /**
     * @brief setLayoutMode
     * 批量布局类型
     * @param mode
     */
    void setLayoutMode(Dr::LayoutMode mode);

    /**
     * @brief setScaleMode
     * 批量缩放类型
     * @param mode
     */
    void setScaleMode(Dr::ScaleMode mode);

    /**
     * @brief setScaleFactor
     * 批量缩放因子;base 1;设置后自动取消自适应
     * @param scaleFactor
     */
    void setScaleFactor(qreal scaleFactor);     //

    /**
     * @brief setMouseShape
     * 设置鼠标样式
     * @param mode
     */
    void setMouseShape(Dr::MouseShape shape);

    /**
     * @brief setAnnotationInserting
     * 设置注释插入状态;设置后点击文档可以在鼠标位置插入图标注释
     * @param inserting
     */
    void setAnnotationInserting(bool inserting);

    /**
     * @brief openMagnifier
     * 调用browser打开放大镜
     */
    void openMagnifier();

    /**
     * @brief closeMagnifier
     * 调用browser关闭放大镜
     */
    void closeMagnifier();

    /**
     * @brief magnifierOpened
     * 获取放大镜是否被打开
     * @return
     */
    bool magnifierOpened();

    /**
     * @brief fileChanged
     * 文档是否被改变
     * @return 是否被改变
     */
    bool fileChanged();

    /**
     * @brief saveData
     * 保存文档
     * @return 是否成功
     */
    bool saveData();

    /**
     * @brief saveData
     * 另存为文档
     * @param 另存为路径
     * @return 是否成功
     */
    bool saveAsData(QString filePath);

    /**
     * @brief handleSearch
     * 通知browser进入搜索模式
     */
    void handleSearch();

    /**
     * @brief stopSearch
     * 通知browser通知搜索
     */
    void stopSearch();

    /**
     * @brief copySelectedText
     * 复制选中文字到剪贴板
     */
    void copySelectedText();

    /**
     * @brief highlightSelectedText
     * 高亮选中文字
     */
    void highlightSelectedText();

    /**
     * @brief addSelectedTextHightlightAnnotation
     * 对选中文字添加高亮注释
     */
    void addSelectedTextHightlightAnnotation();

    bool getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio, bool bSrc = false);

    void defaultFocus();

    QList<deepin_reader::Annotation *> annotations();

    bool removeAnnotation(deepin_reader::Annotation *annotation, bool tips = true);

    bool removeAllAnnotation();

    QList<qreal> scaleFactorList();

    qreal maxScaleFactor();

    QString filter();

    QString format();

    QSet<int> getBookMarkList() const;

    SheetOperation operation() const;

    Dr::FileType fileType();

    QString filePath();

    bool hasBookMark(int index);

    void zoomin();  //放大一级

    void zoomout();

    bool sideBarVisible();

    void setSidebarVisible(bool isVisible, bool notify = true);

    void handleOpenSuccess();

    void openSlide();

    void closeSlide();

    bool isFullScreen();

    void openFullScreen();

    void closeFullScreen();

    void setDocumentChanged(bool changed);

    void setBookmarkChanged(bool changed);

    void setOperationChanged();

    void handleFindNext();

    void handleFindPrev();

    void handleFindExit();

    void handleFindContent(const QString &strFind);

    void showEncryPage();

    bool tryPassword(QString password);

    bool needPassword();

    bool isUnLocked();

    int getPageLableIndex(const QString);

private:
    SheetOperation &operationRef();

public slots:
    void showTips(const QString &tips, int iconIndex = 0);

private slots:
    void onFindContentComming(const deepin_reader::SearchResult &);

    void onFindFinished();

public:
    int label2pagenum(QString label);

    bool haslabel();

    void docBasicInfo(deepin_reader::FileInfo &tFileInfo);

    QString pagenum2label(const int index);

signals:
    void sigFindOperation(const int &);

signals:
    void sigFileChanged(DocSheet *);         //被修改了 书签 注释等

    void sigOperationChanged(DocSheet *);    //被修改了 缩放 当前页等

    void sigOpened(DocSheet *, bool);

private slots:
    void onBrowserPageChanged(int page);

    void onBrowserPageFirst();

    void onBrowserPagePrev();

    void onBrowserPageNext();

    void onBrowserPageLast();

    void onBrowserBookmark(int index, bool state);

    void onBrowserOperaAnnotation(int, int, deepin_reader::Annotation *);

    void onExtractPassword(const QString &password);

protected:
    void resizeEvent(QResizeEvent *event) override;

    void childEvent(QChildEvent *c) override;

private:
    SheetOperation  m_operation;
    QSet<int>       m_bookmarks;

    SheetSidebar   *m_sidebar = nullptr;
    SheetBrowser   *m_browser = nullptr;

    QString         m_filePath;
    Dr::FileType    m_fileType;
    QString         m_uuid;

    bool m_documentChanged = false;
    bool m_bookmarkChanged = false;

    EncryptionPage  *m_encryPage = nullptr;
};

#endif // DocSheet_H
