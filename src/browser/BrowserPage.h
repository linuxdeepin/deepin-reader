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

#include "../document/PDFModel.h"

using namespace deepin_reader;

namespace deepin_reader {
class Annotation;
class Page;
}

class SheetBrowser;
class BrowserWord;
class BrowserAnnotation;
class BrowserPage : public QGraphicsItem
{
    friend class BrowserSearch;
    friend class RenderPageThread;
    friend class RenderViewportThread;
public:
    explicit BrowserPage(SheetBrowser *parent, deepin_reader::Page *page);

    ~BrowserPage() override;

    void reOpen(deepin_reader::Page *page);

    QRectF boundingRect()const override;

    QRectF bookmarkRect();

    QRectF bookmarkMouseRect();

    void setBookmark(bool hasBookmark);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void renderViewPort(bool force = false);     //优先显示当前窗口

    void render(double scaleFactor, Dr::Rotation rotation, bool renderLater = false);

    QImage getImage(double scaleFactor, Dr::Rotation rotation, const QRect &boundingRect = QRect());

    QImage getImage(int width, int height, Qt::AspectRatioMode mode, bool bSrc); //按宽高缩放

    QImage thumbnail() ;

    QImage getImageRect(double scaleFactor, QRect rect);//获取缩放后的局部区域图片

    QImage getImagePoint(double scaleFactor, QPoint point);

    QImage getCurImagePoint(QPoint point);

    void handlePreRenderFinished(Dr::Rotation rotation, QImage image);

    void handleRenderFinished(double scaleFactor, Dr::Rotation rotation, QImage image, QRect rect = QRect());

    void handleViewportRenderFinished(double scaleFactor, Dr::Rotation rotation, QImage image, QRect rect = QRect());

    static bool existInstance(BrowserPage *item);

    void setItemIndex(int itemIndex);

    int itemIndex();

    QString selectedWords();

    void setWordSelectable(bool selectable);

    void clearWords();

    void loadWords();

    void scaleWords();

    QList<BrowserWord *> loadPageWord();

    void loadAnnotations();     //如果加载过则不加载

    QList< deepin_reader::Annotation * > annotations();

    bool updateAnnotation(deepin_reader::Annotation *annotation, const QString, const QColor);

    Annotation *addHighlightAnnotation(QString text, QColor color);

    bool hasAnnotation(deepin_reader::Annotation *annotation);

    bool removeAnnotation(deepin_reader::Annotation *annotation);

    bool removeAnnotationByUniqueName(QString uniqueName);

    Annotation *addIconAnnotation(const QRectF, const QString);

    bool mouseClickIconAnnot(QPointF &);

    void setSearchHighlightRectf(const QList< QRectF > &rectflst);

    void clearSearchHighlightRects();

    void translate2NormalRect(QRectF &);

    void clearSelectSearchHighlightRects();

    int searchHighlightRectSize();

    QRectF findSearchforIndex(int index);

    QRectF translateRect(const QRectF &rect);

    BrowserAnnotation *getBrowserAnnotation(const QPoint &point);

    BrowserWord *getBrowserWord(const QPoint &point);

    void setSelectIconRect(const bool draw, Annotation *iconAnnot = nullptr);

    void setDrawMoveIconRect(const bool draw);

    void setIconMovePos(const QPointF);

    QString deleteNowSelectIconAnnotation();

    bool moveIconAnnotation(const QRectF);

private:
    void reloadAnnotations();

protected:
    bool sceneEvent(QEvent *event) override;

    struct ImageFragment {
        QRect rect;
        QImage image;
    };

private:
    deepin_reader::Page *m_page = nullptr;

    double  m_scaleFactor        = -1;       //当前被设置的缩放

    QPixmap m_pixmap;                       //当前图片
    bool    m_pixmapHasRendered = false;    //当前图片是否已经开始加载
    double  m_pixmapScaleFactor   = -1;     //当前图片的缩放
    QRect   m_pixmapRenderedRect;           //当前图片已经加载的rect

    QPixmap m_viewportPixmap;               //视图区域的图片
    QRect   m_viewportRenderedRect;         //试图区域

    bool m_bookmark = false;
    int m_bookmarkState = 0;   //1为on 2为pressed 3为show
    int m_index = 0;
    Dr::Rotation m_rotation = Dr::NumberOfRotations;
    static QSet<BrowserPage *> items;
    SheetBrowser *m_parent = nullptr;

    Dr::Rotation m_wordRotation = Dr::NumberOfRotations;
    double m_wordScaleFactor = -1;
    QList<BrowserWord *> m_words;

    bool m_hasLoadedAnnotation = false;
    QList<BrowserAnnotation *> m_annotationItems;  //一个注释可能对应多个annotationitems
    QList<deepin_reader::Annotation *> m_annotations;

    QPointF m_posPressed;
    bool m_wordSelectable = false;

    QRectF m_searchSelectLighRectf;
    QList<QRectF> m_searchLightrectLst;

    QMutex m_imageMutex;

    BrowserAnnotation *m_lastClickIconAnnotation{nullptr};
    bool m_drawIconRect{false}; // 绘制当前选中图标注释边框
    QRectF m_selecetIconAnnotationRect{QRectF()}; // 绘制选择的图标注释的外边框
    bool m_drawMoveIconRect{false}; // 绘制移动图标注释边框
    QPointF m_drawMoveIconPoint{QPointF()}; // 绘制移动图标注释点
};

#endif // BrowserPage_H
