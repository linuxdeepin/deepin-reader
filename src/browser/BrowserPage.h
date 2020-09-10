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

#include "Global.h"
#include "PDFModel.h"

#include <QGraphicsItem>
#include <QSet>
#include <QMutex>

using namespace deepin_reader;

namespace deepin_reader {
class Annotation;
class Page;
}

class SheetBrowser;
class BrowserWord;
class BrowserAnnotation;

/**
 * @brief The BrowserPage class
 * 用于视图框架中表示一页文档
 */
class BrowserPage : public QGraphicsItem
{
    friend class PageSearchThread;
    friend class PageRenderThread;
    friend class PageViewportThread;
public:
    explicit BrowserPage(SheetBrowser *parent, deepin_reader::Page *page, QList<deepin_reader::Page *> renderPages);

    virtual ~BrowserPage() override;

    /**
     * @brief existInstance
     * 页是否存在
     * @param item 哪一页
     * @return
     */
    static bool existInstance(BrowserPage *item);

    /**
     * @brief reOpen
     * 重新对自己赋值
     * @param page
     * @param renderPages 渲染页
     */
    void reOpen(deepin_reader::Page *page, QList<deepin_reader::Page *> renderPages);

    /**
     * @brief boundingRect
     * 文档页rect(不一定是0度下的)
     * @return
     */
    QRectF boundingRect()const override;  //原矩形 不受旋转影响

    /**
     * @brief rect
     * 文档页rect(0度)
     * @return
     */
    QRectF rect();  //旋转后 实际矩形

    /**
     * @brief setBookmark
     * 设置书签
     * @param hasBookmark
     */
    void setBookmark(const bool &hasBookmark);

    /**
     * @brief updateBookmarkState
     * 更新书签状态
     */
    void updateBookmarkState();

    /**
     * @brief render
     * 文档缩略图渲染接口
     * @param scaleFactor 缩放系数
     * @param rotation 旋转角度
     * @param renderLater
     * @param force
     */
    void render(const double &scaleFactor, const Dr::Rotation &rotation, const bool &renderLater = false, const bool &force = false);

    /**
     * @brief renderViewPort
     * 优先显示当前窗口
     * @param force
     */
    void renderViewPort(bool force = false);

    /**
     * @brief thumbnail
     * 文档页的缩略图(大部分获取是空的)
     * @return 缩略图
     */
    QImage thumbnail();

    /**
     * @brief getImage
     * 获取该缩放后的图片中的一部分
     * @param scaleFactor 缩放因子
     * @param rotation 旋转
     * @param boundingRect 范围 默认为获取全部
     * @param renderIndex 选择线程 -1默认
     * @return
     */
    QImage getImage(double scaleFactor, Dr::Rotation rotation, const QRect &boundingRect = QRect(), int renderIndex = -1);

    /**
     * @brief getImage
     * 获取整个图片并转成对应宽高
     * @param width 宽
     * @param height 高
     * @param mode 缩放模式
     * @param bSrc 是否可以使用已存在图片缩放
     * @param renderIndex 选择线程 -1默认
     * @return
     */
    QImage getImage(int width, int height, Qt::AspectRatioMode mode, bool bSrc, int renderIndex = -1);

    /**
     * @brief getImageRect
     * 得到文档缩放之后的图
     * @param scaleFactor 缩放系数
     * @param rect 范围
     * @return  文档缩放之后的图
     */
    QImage getImageRect(double scaleFactor, QRect rect);//获取缩放后的局部区域图片

    /**
     * @brief getImagePoint
     * 得到鼠标点周围的片图(放大镜)
     * @param scaleFactor 缩放比例
     * @param point 鼠标点
     * @return鼠标点周围的片图(带缩放)
     */
    QImage getImagePoint(double scaleFactor, QPoint point);

    /**
     * @brief getCurImagePoint
     * 得到鼠标点周围的片图(放大镜)
     * @param point 鼠标点
     * @return 鼠标点周围的片图
     */
    QImage getCurImagePoint(QPoint point);

    /**
     * @brief getWords
     * 得到文档无旋转下的左右文字
     * @return 文字列表
     */
    QList<Word> getWords();

    /**
     * @brief setItemIndex
     * 设置文档页的编号
     * @param itemIndex 编号
     */
    void setItemIndex(int itemIndex);

    /**
     * @brief itemIndex
     * 当前页的编号(从0开始)
     * @return
     */
    int itemIndex();

    /**
     * @brief selectedWords
     * 得到当前选择文字内容
     * @return  选择文字的内容
     */
    QString selectedWords();

    /**
     * @brief setWordSelectable
     * 设置文字是否可选择
     * @param selectable true:可选   false:不可选
     */
    void setWordSelectable(bool selectable);

    /**
     * @brief loadWords
     * 加载文字 如果正在加载则不进行操作 如果已经加载过则按当前缩放改变大小
     */
    void loadWords();

    /**
     * @brief loadAnnotations
     * 加载注释 如果已经加载则不再加载
     */
    void loadAnnotations();     //如果加载过则不加载

    /**
     * @brief scaleWords
     *  更改缩放如果存在文字
     */
    void scaleWords(bool force = false);

    /**
     * @brief clearPixmap
     * 删除缓存图片
     */
    void clearPixmap();

    /**
     * @brief clearWords
     *  清除文字 被选中除外
     */
    void clearWords();

    /**
     * @brief annotations
     * 当前页中所有注释
     * @return 注释列表
     */
    QList< deepin_reader::Annotation * > annotations();

    /**
     * @brief updateAnnotation
     * 更新注释内容,颜色
     * @param annotation 当前注释
     * @param text 注释内容
     * @param color 注释高亮颜色
     * @return true:更新成功  false:更新失败
     */
    bool updateAnnotation(deepin_reader::Annotation *annotation, const QString, const QColor);

    /**
     * @brief addHighlightAnnotation
     * 添加文本高亮注释接口
     * @param text  高亮注释内容
     * @param color 高亮注释颜色
     * @return      当前添加高亮注释
     */
    Annotation *addHighlightAnnotation(QString text, QColor color);

    /**
     * @brief hasAnnotation
     * 当前页注释中是否有这个注释
     * @param annotation 当前注释
     * @return true:有       false:没有
     */
    bool hasAnnotation(deepin_reader::Annotation *annotation);

    /**
     * @brief removeAnnotation
     * 删除某个注释
     * @param annota 具体哪个注释
     * @return true:删除成功, false:删除失败
     */
    bool removeAnnotation(deepin_reader::Annotation *annotation);

    /**
     * @brief removeAnnotationByUniqueName
     * 根据序列号删除注释
     * @param uniqueName 序列号
     * @return true:删除成功, false:删除失败
     */
    bool removeAnnotationByUniqueName(QString uniqueName);

    /**
     * @brief addIconAnnotation
     * 添加图标注释
     * @param rect 图标rect
     * @param text 注释内容
     * @return 添加之后的注释,添加失败返回nullptr,反之不为nullptr
     */
    Annotation *addIconAnnotation(const QRectF, const QString);

    /**
     * @brief mouseClickIconAnnot
     * 当前鼠标点击位置是否有注释
     * @param clickPoint 鼠标点击位置
     * @return true:有  false:没有
     */
    bool mouseClickIconAnnot(QPointF &);

    /**
     * @brief setSearchHighlightRectf
     * 设置当前搜索选择框,在搜索列表中
     * @param rectflst
     */
    void setSearchHighlightRectf(const QList< QRectF > &rectflst);

    /**
     * @brief clearSearchHighlightRects
     *结束搜索,清除搜索选择框,清空搜索结果列表
     */
    void clearSearchHighlightRects();

    /**
     * @brief clearSelectSearchHighlightRects
     * 清除高亮选择框,在搜索结果中
     */
    void clearSelectSearchHighlightRects();

    /**
     * @brief searchHighlightRectSize
     * 当前页中搜索到目标的个数
     * @return 目标的个数
     */
    int searchHighlightRectSize();

    /**
     * @brief findSearchforIndex
     * 得到搜索元素的rect,根据编号
     * @param index 编号
     * @return 编号对应的rect
     */
    QRectF findSearchforIndex(int index);

    /**
     * @brief translatePoint
     * 将非0度(旋转角度)坐标点转换成0度坐标点
     * @param point 转换之前坐标点
     * @return 转换之后坐标点
     */
    QPoint translatePoint(const QPoint &point);

    /**
     * @brief translateRect
     * 将非0度(旋转角度)rect转换成0度rect
     * @param rect 转换之前rect
     * @return 转换之后rect
     */
    QRectF translateRect(const QRectF &rect);

    /**
     * @brief getNorotateRecr
     * 得到没有旋转rect
     * @param rect
     * @return
     */
    QRectF getNorotateRect(const QRectF &rect);

    /**
     * @brief getBrowserAnnotation
     * 获得注释,根据坐标点
     * @param point 坐标点
     * @return 坐标点下的注释
     */
    BrowserAnnotation *getBrowserAnnotation(const QPoint &point);

    /**
     * @brief getBrowserWord
     * 根据坐标点获得当前的文字
     * @param point 当前坐标点
     * @return 坐标点下的文字
     */
    BrowserWord *getBrowserWord(const QPoint &point);

    /**
     * @brief setSelectIconRect
     * 设置图标注释选择框
     * @param draw true:绘制  false:取消绘制
     * @param iconAnnot 图标注释
     */
    void setSelectIconRect(const bool draw, Annotation *iconAnnot = nullptr);

    /**
     * @brief setDrawMoveIconRect
     * 绘制图标注释选择框
     * @param draw true:绘制  false:取消绘制
     */
    void setDrawMoveIconRect(const bool draw);

    /**
     * @brief setIconMovePos
     * 图标注释选择框移动位置
     * @param movePoint 移动位置
     */
    void setIconMovePos(const QPointF);

    /**
     * @brief deleteNowSelectIconAnnotation
     * 删除当前选中的图标注释
     * @return
     */
    QString deleteNowSelectIconAnnotation();

    /**
     * @brief moveIconAnnotation
     * 移动注释图标位置
     * @return true:移动成功   false:移动失败
     */
    bool moveIconAnnotation(const QRectF);

    /**
     * @brief removeAllAnnotation
     * 删除当前页中所有注释
     * @return ture:删除成功       false:删除失败
     */
    bool removeAllAnnotation();

    /**
     * @brief jump2Link
     * 跳转到相应link处(如果是目录跳到文档正文处,如果是网址会在浏览器中打开)
     * @param point 当前鼠标点
     * @return true:跳转成功    false:跳转失败
     */
    bool jump2Link(const QPointF);

    /**
     * @brief inLink
     * 当前鼠标位置是否有link
     * @param pos 当前鼠标位置
     * @return true:有  false:没有
     */
    bool inLink(const QPointF);

    /**
     * @brief setPageBookMark
     * 在书签附近文字时,有时添加/删除书签无效,特意在brower中先处理
     * @param clickPoint 鼠标点击位置
     */
    void setPageBookMark(const QPointF);

private:
    /**
     * @brief handleRenderFinished
     * 渲染缩略图
     * @param scaleFactor 缩放系数
     * @param image 缩略图
     * @param rect 范围
     */
    void handleRenderFinished(const double &scaleFactor, const QImage &image, const QRect &rect = QRect());

    void handleViewportRenderFinished(const double &scaleFactor, const QImage &image, const QRect &rect = QRect());

    /**
     * brief handleWordLoaded
     * 加载文字
     * @param words 要加载的文字
     */
    void handleWordLoaded(const QList<Word> &words);

    /**
     * @brief reloadAnnotations
     * 重新加载页中所有注释
     */
    void reloadAnnotations();

    /**
     * @brief bookmarkRect
     * 书签范围
     * @return
     */
    QRectF bookmarkRect();

    /**
     * @brief bookmarkMouseRect
     * 书签图标范围
     * @return
     */
    QRectF bookmarkMouseRect();

    /**
     * @brief updatePageFull
     * 更新整页
     */
    void updatePageFull();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    bool sceneEvent(QEvent *event) override;

private:
    static QSet<BrowserPage *> items;                   //用于记录多少个自己

    SheetBrowser *m_parent = nullptr;

    deepin_reader::Page *m_page = nullptr;              //主要操作更新
    QList<deepin_reader::Page *> m_renderPages;         //用来专注于渲染的副本

    deepin_reader::AnnotationList m_annotations;   //
    deepin_reader::AnnotationList m_annotations0;  // 渲染page的注释  之后改成QList<deepin_reader::m_annotations> m_renderAnnotations
    deepin_reader::AnnotationList m_annotations1;  //
    deepin_reader::AnnotationList m_annotations2;  //
    deepin_reader::AnnotationList m_annotations3;  //

    int     m_index = 0;                                //当前索引
    double  m_scaleFactor = -1;                         //当前被设置的缩放
    Dr::Rotation m_rotation = Dr::NumberOfRotations;    //当前被设置的旋转

    QPixmap m_pixmap;                       //当前图片
    bool    m_pixmapHasRendered = false;    //当前图片是否已经开始加载
    double  m_pixmapScaleFactor   = -1;     //当前图片的缩放
    QRect   m_pixmapRenderedRect;           //当前图片已经加载的rect

    bool    m_viewportTryRender = false;    //视图区域绘制尝试过调用
    double  m_viewportScaleFactor = -1;     //视图区域的缩放
    QPixmap m_viewportPixmap;               //视图区域的图片
    QRect   m_viewportRenderedRect;         //试图区域

    QList<BrowserWord *> m_words;                           //当前文字
    double m_wordScaleFactor = -1;                          //当前文字的缩放
    bool m_wordIsRendering = false;                         //当前文字是否正在加载
    bool m_wordHasRendered = false;                         //当前文字是否被加载
    bool m_wordNeeded      = false;                         //当前文字是否需要
    bool m_wordSelectable  = false;                         //当前文字是否可以选取

    QList<BrowserAnnotation *> m_annotationItems;           //一个deepin_reader::Annotation可能对应多个annotationItems
    BrowserAnnotation *m_lastClickIconAnnotationItem = nullptr;
    bool m_hasLoadedAnnotation = false;                     //是否已经加载注释
    bool m_drawMoveIconRect = false;                        // 绘制移动图标注释边框
    QPointF m_drawMoveIconPoint;                            // 绘制移动图标注释点

    QList<QRectF> m_searchLightrectLst;                     //搜索结果
    QRectF m_searchSelectLighRectf;

    bool m_bookmark = false;                                // 当前是否有书签
    int  m_bookmarkState = 0;                               // 当前书签状态 1为on 2为pressed 3为show
};

#endif // BrowserPage_H
