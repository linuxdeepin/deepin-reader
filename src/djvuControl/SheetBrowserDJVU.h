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
#ifndef SHEETBROWSERDJVU_H
#define SHEETBROWSERDJVU_H

#include <QGraphicsView>
#include "document/model.h"
#include "global.h"
#include <QLabel>

class DocOperation;
class SheetBrowserDJVUItem;
class DocSheetDJVU;
class SheetBrowserDJVU : public QGraphicsView
{
    Q_OBJECT
public:
    explicit SheetBrowserDJVU(DocSheetDJVU *parent = nullptr);

    ~SheetBrowserDJVU() override;

    bool openFilePath(const QString &);

    bool loadPages(DocOperation &operation, const QSet<int> &bookmarks);

    void loadMouseShape(DocOperation &operation);

    void setBookMark(int index, int state);

    void deform(DocOperation &operation);      //根据当前参数进行变换

    bool hasLoaded();

    int allPages();

    int currentPage();

    int  viewPointInIndex(QPoint viewPoint);

    void setCurrentPage(int page);

    bool getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio);

    void openMagnifier();

    void closeMagnifier();

    bool magnifierOpened();

    int maxWidth();

    int maxHeight();

    void needBookmark(int index, bool state);

signals:
    void sigPageChanged(int page);

    void sigWheelUp();

    void sigWheelDown();

    void sigSizeChanged();

    void sigNeedPageFirst();

    void sigNeedPagePrev();

    void sigNeedPageNext();

    void sigNeedPageLast();

    void sigNeedBookMark(int index, bool state);

protected:
    void showEvent(QShowEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);

    void wheelEvent(QWheelEvent *event);

    bool getImagePoint(QPoint viewPoint, double scaleFactor, QImage &image);

private slots:
    void onVerticalScrollBarValueChanged(int value);

    void onCustomContextMenuRequested(const QPoint &);

private:
    deepin_reader::Document *m_document = nullptr;
    QList<SheetBrowserDJVUItem *> m_items;
    double m_lastScaleFactor = 0;
    int m_maxWidth = 0;         //最大一页的宽度
    int m_maxHeight = 0;        //最大一页的高度
    bool m_hasLoaded = false;   //是否已经加载过每页的信息
    int m_initPage = 1;         //用于刚显示跳转的页数
    QLabel *m_magnifierLabel = nullptr;
    DocSheetDJVU *m_sheet = nullptr;
};

#endif // SHEETBROWSERDJVU_H
