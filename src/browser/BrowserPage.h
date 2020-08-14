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
#ifndef BrowserPage_H
#define BrowserPage_H

#include <QGraphicsItem>
#include <QSet>
#include <QMutex>

#include "Global.h"
#include "PDFModel.h"

using namespace deepin_reader;

namespace deepin_reader {
class Annotation;
class Page;
}

class SheetBrowser;
class BrowserWord;
class BrowserLink;
class BrowserAnnotation;
class BrowserPage : public QGraphicsItem
{
    friend class PageSearchThread;
    friend class PageRenderThread;
    friend class PageViewportThread;
public:
    explicit BrowserPage(SheetBrowser *parent, deepin_reader::Page *page, QList<deepin_reader::Page *> renderPages);

    virtual ~BrowserPage() override;

    static bool existInstance(BrowserPage *item);

    void reOpen(deepin_reader::Page *page, QList<deepin_reader::Page *> renderPages);

    QRectF boundingRect()const override;  //原矩形 不受旋转影响

    QRectF rect();  //旋转后 实际矩形

    void setBookmark(const bool &hasBookmark);

    void updateBookmarkState();

    void render(const double &scaleFactor, const Dr::Rotation &rotation, const bool &renderLater = false, const bool &force = false);

    void renderViewPort(bool force = false);     //优先显示当前窗口

    QImage getImage(double scaleFactor, Dr::Rotation rotation, const QRect &boundingRect = QRect(), int renderInder = -1);

    QImage getImage(int width, int height, Qt::AspectRatioMode mode, bool bSrc, int renderIndex = -1); //按宽高缩放

    QImage thumbnail() ;

    QImage getImageRect(double scaleFactor, QRect rect);//获取缩放后的局部区域图片

    QImage getImagePoint(double scaleFactor, QPoint point);

    QImage getCurImagePoint(QPoint point);

    QList<Word> getWords();

    void setItemIndex(int itemIndex);

    int itemIndex();

    QString selectedWords();

    void setWordSelectable(bool selectable);

    void loadLinks();

    void loadWords();

    void hideWords();

    void scaleWords(bool force = false);

    void loadAnnotations();     //如果加载过则不加载

    QList< deepin_reader::Annotation * > annotations();

    bool updateAnnotation(deepin_reader::Annotation *annotation, const QString, const QColor);

    Annotation *addHighlightAnnotation(QString text, QColor color);

    bool hasAnnotation(deepin_reader::Annotation *annotation);

    bool removeAnnotation(deepin_reader::Annotation *annotation);

    bool removeAnnotationByUniqueName(QString uniqueName);

    //================================
    Annotation *addIconAnnotation(const QRectF, const QString);

    bool mouseClickIconAnnot(QPointF &);

    void setSearchHighlightRectf(const QList< QRectF > &rectflst);

    void clearSearchHighlightRects();

    void translate2NormalRect(QRectF &);

    void clearSelectSearchHighlightRects();

    int searchHighlightRectSize();

    QRectF findSearchforIndex(int index);

    QPoint translatePoint(const QPoint &point);

    QRectF translateRect(const QRectF &rect);

    QRectF getNorotateRecr(const QRectF &rect);

    BrowserAnnotation *getBrowserAnnotation(const QPoint &point);

    BrowserWord *getBrowserWord(const QPoint &point);

    void setSelectIconRect(const bool draw, Annotation *iconAnnot = nullptr);

    void setDrawMoveIconRect(const bool draw);

    void setIconMovePos(const QPointF);

    QPointF translate2LocalPos(const QPointF);

    QString deleteNowSelectIconAnnotation();

    bool moveIconAnnotation(const QRectF);

    bool removeAllAnnotation();

    bool jump2Link(const QPointF);

    bool inLink(const QPointF);

private:
    void handleRenderFinished(const double &scaleFactor, const QImage &image, const QRect &rect = QRect());

    void handleViewportRenderFinished(const double &scaleFactor, const QImage &image, const QRect &rect = QRect());

    void handleWordLoaded(const QList<Word> &words);

    void reloadAnnotations();

    QRectF bookmarkRect();

    QRectF bookmarkMouseRect();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    bool sceneEvent(QEvent *event) override;

private:
    static QSet<BrowserPage *> items;                   //用于记录多少个自己
    deepin_reader::Page *m_page = nullptr;              //主要操作更新
    QList<deepin_reader::Page *> m_renderPages;         //用来专注于渲染
    QList<deepin_reader::Annotation *> m_annotations;   //
    QList<deepin_reader::Annotation *> m_annotations0;  //渲染page的注释
    QList<deepin_reader::Annotation *> m_annotations1;  //
    QList<deepin_reader::Annotation *> m_annotations2;  //
    QList<deepin_reader::Annotation *> m_annotations3;  //

    SheetBrowser *m_parent = nullptr;

    int     m_index = 0;                                //当前索引
    double  m_scaleFactor = -1;                         //当前被设置的缩放
    Dr::Rotation m_rotation = Dr::NumberOfRotations;    //当前被设置的旋转

    QPixmap m_pixmap;                     //当前图片
    bool    m_pixmapHasRendered = false;  //当前图片是否已经开始加载
    double  m_pixmapScaleFactor   = -1;   //当前图片的缩放
    QRect   m_pixmapRenderedRect;         //当前图片已经加载的rect

    bool    m_viewportTryRender = false;  //视图区域绘制尝试过调用
    double  m_viewportScaleFactor = -1;
    QPixmap m_viewportPixmap;       //视图区域的图片
    QRect   m_viewportRenderedRect; //试图区域

    bool m_wordHasRendered = false;                         //当前文字是否被加载
    QList<BrowserWord *> m_words;                           //当前文字
    double m_wordScaleFactor = -1;                          //当前文字的缩放

    bool m_bookmark = false;   //当前是否有书签
    int  m_bookmarkState = 0;  //当前书签状态 1为on 2为pressed 3为show

    bool m_hasLoadedAnnotation = false;                 //是否已经加载注释
    QList<BrowserAnnotation *> m_annotationItems;       //一个注释可能对应多个annotationitems


    QList<BrowserLink *> m_linkItems;

    QPointF m_posPressed;               //是否正在按下
    bool m_wordIsHide = false;
    bool m_wordSelectable = false;      //文字是否可以选取

    QRectF m_searchSelectLighRectf;
    QList<QRectF> m_searchLightrectLst;

    BrowserAnnotation *m_lastClickIconAnnotation{nullptr};
    bool m_drawIconRect{false};                         // 绘制当前选中图标注释边框
    QRectF m_selecetIconAnnotationRect{QRectF()};       // 绘制选择的图标注释的外边框
    bool m_drawMoveIconRect{false};                     // 绘制移动图标注释边框
    QPointF m_drawMoveIconPoint{QPointF()};             // 绘制移动图标注释点
};

#endif // BrowserPage_H
