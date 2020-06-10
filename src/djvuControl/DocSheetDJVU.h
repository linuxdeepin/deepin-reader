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
#ifndef DOCSHEETDJVU_H
#define DOCSHEETDJVU_H

#include "DocSheet.h"

class SheetBrowserDJVU;
class DocSheetDJVU : public DocSheet
{
    Q_OBJECT
    Q_DISABLE_COPY(DocSheetDJVU)

public:
    explicit DocSheetDJVU(QString filePath, QWidget *parent = nullptr);

    virtual ~DocSheetDJVU()override;

    bool openFileExec()override;

    static QImage firstThumbnail(const QString &filePath);

    void setBookMark(int index, int state) override;

    bool fileChanged()override;

    bool saveData()override;

    bool saveAsData(QString filePath)override;

    int pagesNumber()override;

    int currentPage()override;      //从1开始

    int currentIndex()override;     //从0开始

    void jumpToPage(int page)override;

    void jumpToIndex(int index)override;

    void jumpToFirstPage()override;

    void jumpToLastPage()override;

    void jumpToNextPage()override;

    void jumpToPrevPage()override;

    void rotateLeft()override;

    void rotateRight()override;

    void setLayoutMode(Dr::LayoutMode mode)override;

    void setScaleFactor(qreal scaleFactor)override;

    void setMouseShape(Dr::MouseShape shape)override;

    void setScaleMode(Dr::ScaleMode mode)override;

    bool getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio) override;

    void openMagnifier()override;

    void closeMagnifier()override;

    void defaultFocus()override;

    bool magnifierOpened()override;

    void docBasicInfo(stFileInfo &info)override;

    QString filter()override;

private slots:
    void onBrowserPageChanged(int page);

    void onBrowserSizeChanged();

    void onBrowserWheelUp();

    void onBrowserWheelDown();

    void onBrowserPageFirst();

    void onBrowserPagePrev();

    void onBrowserPageNext();

    void onBrowserPageLast();

    void onBrowserBookmark(int index, bool state);

private:
    SheetBrowserDJVU *m_browser = nullptr;
    bool m_fileChanged = false;
};

#endif // DOCSHEETDJVU_H
