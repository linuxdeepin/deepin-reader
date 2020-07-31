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
#ifndef SheetBrowser_H
#define SheetBrowser_H

#include <DGraphicsView>
#include <QLabel>
#include <QMutex>

#include "document/Model.h"
#include "Global.h"

using namespace deepin_reader;

class SheetOperation;
class BrowserPage;
class DocSheet;
class TipsWidget;
class BrowserAnnotation;
class NoteShadowViewWidget;
class RenderViewportThread;
class BrowserMagniFier;
class FindWidget;
class PageSearchThread;
class SheetBrowser : public Dtk::Widget::DGraphicsView
{
    Q_OBJECT
public:
    friend class BrowserMagniFier;
    friend class ReadMagnifierManager;

    explicit SheetBrowser(DocSheet *parent = nullptr);

    virtual ~SheetBrowser() override;

    static QImage firstThumbnail(const QString &filePath);

    bool isUnLocked();

    bool open(const Dr::FileType &fileType, const QString &filePath, const QString &password);

    bool reOpen(const Dr::FileType &fileType, const QString &filePath);

    bool save();

    bool saveAs(const QString &filePath);

    bool loadPages(SheetOperation &operation, const QSet<int> &bookmarks);

    void setMouseShape(const Dr::MouseShape &shape);

    void setBookMark(int index, int state);

    void setAnnotationInserting(bool inserting);

    void deform(SheetOperation &operation);      //根据当前参数进行变换

    bool hasLoaded();

    int allPages();

    int currentPage();

    int  viewPointInIndex(QPoint viewPoint);

    void setCurrentPage(int page);

    bool getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio, bool bSrc = false);

    void openMagnifier();

    void closeMagnifier();

    bool magnifierOpened();

    int maxWidth();

    int maxHeight();

    void needBookmark(int index, bool state);

    QString selectedWordsText();

    void handleVerticalScrollLater();

    deepin_reader::Outline outline();

    Properties properties() const;

    void jumpToOutline(const qreal  &left, const qreal &top, unsigned int page);

    void jumpToHighLight(deepin_reader::Annotation *annotation, const int index);

    QList< deepin_reader::Annotation * > annotations();

    Annotation *addIconAnnotation(BrowserPage *page, const QPointF, const QString);

    Annotation *addHighLightAnnotation(const QString, const QColor, QPoint &);

    bool removeAnnotation(deepin_reader::Annotation *annotation);

    bool removeAllAnnotation();

    bool updateAnnotation(deepin_reader::Annotation *annotation, const QString &text, QColor color = QColor());

    void showNoteEditWidget(deepin_reader::Annotation *annotation, const QPoint &point);

    void handleSearch();

    void stopSearch();

    void handleFindNext();

    void handleFindPrev();

    void handleFindExit();

    void handleFindContent(const QString &strFind);

    void handleFindFinished(int searchcnt);

    bool jump2Link(const QPointF);

signals:
    void sigPageChanged(int page);

    void sigWheelUp();

    void sigWheelDown();

    void sigNeedPageFirst();

    void sigNeedPagePrev();

    void sigNeedPageNext();

    void sigNeedPageLast();

    void sigNeedBookMark(int index, bool state);

    void sigOperaAnnotation(int type, deepin_reader::Annotation *annotation);

    void sigPartThumbnailUpdated(int index);

    void sigThumbnailUpdated(int index);

    void sigAddHighLightAnnot();

    void sigAddHighLightAnnot(BrowserPage *, QString, QColor);

protected:
    void showEvent(QShowEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    BrowserPage *mouseClickInPage(QPointF &);

    BrowserPage *getBrowserPageForPoint(QPoint &viewPoint);

private slots:
    void onVerticalScrollBarValueChanged(int value);

    void onHorizontalScrollBarValueChanged(int value);

    void onSceneOfViewportChanged();

    void onAddHighLightAnnot(BrowserPage *, QString, QColor);

    void onRemoveAnnotation(deepin_reader::Annotation *annotation, bool tips);

    void onUpdateAnnotation(deepin_reader::Annotation *annotation, const QString &text);

    void onInit();

private:
    bool mouseClickIconAnnot(QPointF &);

    bool calcIconAnnotRect(BrowserPage *page, const QPointF, QRectF &);

    QPointF translate2Local(const QPointF);

    Annotation *getClickAnnot(BrowserPage *page, const QPointF, bool drawRect = false);

    void jump2PagePos(BrowserPage *page, const qreal left, const qreal top);

    void addNewIconAnnotDeleteOld(BrowserPage *page, const QPointF);

    int currentScrollValueForPage();

    void curpageChanged(int curpage);

    bool isLink(const QPointF);

    void setIconAnnotSelect(const bool);

private:
    deepin_reader::Document *m_document = nullptr;
    DocSheet *m_sheet = nullptr;

    BrowserMagniFier *m_magnifierLabel = nullptr;
    TipsWidget *m_tipsWidget = nullptr;
    NoteShadowViewWidget *m_noteEditWidget = nullptr;
    FindWidget *m_pFindWidget = nullptr;
    PageSearchThread *m_searchTask = nullptr;
    BrowserPage *m_lastFindPage = nullptr;
    BrowserPage *m_lastSelectIconAnnotPage = nullptr; // 最后选中图标注释所在页
    QTimer *m_resizeTimer = nullptr;        //大小改变触发局部update
    QTimer *m_scrollTimer = nullptr;        //滚动位移触发局部update

    Dr::FileType m_fileType;
    QString m_filePath;
    QString m_filePassword;
    QList<BrowserPage *> m_items;

    QPointF m_selectPressedPos;         //
    QPointF m_selectStartPos;           // 选取文字的开始位置
    QPointF m_selectEndPos;             // 选取文字的结束位置

    double m_lastScaleFactor = 0;
    int m_maxWidth = 0;                 //最大一页的宽度
    int m_maxHeight = 0;                //最大一页的高度
    bool m_changSearchFlag = false;
    bool m_hasLoaded = false;           //是否已经加载过每页的信息
    int m_initPage = 1;                 //用于刚显示跳转的页数
    int m_searchCurIndex = 0;
    int m_searchPageTextIndex = 0;
    int m_selectIndex = -1;              // 选取文字开始的index

    bool m_annotationInserting = false;     //正在插入注释状态
    RenderViewportThread *m_pageThread = nullptr;
    bool m_selectIconAnnotation{false}; // 当前是否有选中的图标注释
    QPointF m_iconAnnotationMovePos;    // 当前选中的图标注释移动位置
    deepin_reader::Annotation *m_iconAnnot = nullptr; // 当前选中的图标注释

    int m_currentPage = 0;
    int m_lastrotation = 0;
    bool m_bNeedNotifyCurPageChanged = true;
};

#endif // SheetBrowser_H
