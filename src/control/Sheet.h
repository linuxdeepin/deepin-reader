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
#ifndef Sheet_H
#define Sheet_H

#include <DSplitter>
#include "document/Model.h"

class SheetSidebar;
class SlideWidget;
struct SheetOperation {
    Dr::LayoutMode layoutMode   = Dr::SinglePageMode;
    Dr::MouseShape mouseShape   = Dr::MouseShapeNormal;
    Dr::ScaleMode scaleMode     = Dr::ScaleFactorMode;
    Dr::Rotation rotation       = Dr::RotateBy0;
    qreal scaleFactor           = 1.0;
    bool sidebarVisible         = false;
    int  sidebarIndex           = 0;
    int  currentPage            = 1;
};

class SheetBrowser;
class Sheet : public Dtk::Widget::DSplitter
{
    Q_OBJECT
    Q_DISABLE_COPY(Sheet)

    friend class Database;
public:
    explicit Sheet(Dr::FileType type, QString filePath, QWidget *parent = nullptr);

    ~Sheet();

public:
    static bool existFileChanged();

    static QUuid getUuid(Sheet *);

    static Sheet *getSheet(QString uuid);

    static void blockShutdown();

    static void unBlockShutdown();

    static QMap<QString, Sheet *> g_map;

public:
    void initOperationData(const SheetOperation &opera);

    bool isOpen();

    void openFile();

    bool openFileExec();

    int pagesNumber();

    int currentPage();      //从1开始

    int currentIndex();     //从0开始

    void jumpToIndex(int index);

    void jumpToPage(int page);

    void jumpToFirstPage();

    void jumpToLastPage();

    void jumpToNextPage();

    void jumpToPrevPage();

    //Outline outline();

    //void jumpToOutline(const qreal  &realleft, const qreal &realtop, unsigned int ipage = 0);

    //void jumpToHighLight(const QString &uuid, int ipage);

    void rotateLeft();

    void rotateRight();

    void setBookMark(int index, int state);

    void setLayoutMode(Dr::LayoutMode mode);

    void setScaleMode(Dr::ScaleMode mode);

    void setScaleFactor(qreal scaleFactor);     //base is 1 ;设置后自动取消自适应

    void setMouseShape(Dr::MouseShape shape);

    void openMagnifier();

    void closeMagnifier();

    bool magnifierOpened();

    bool fileChanged();

    bool saveData();

    bool saveAsData(QString filePath);

    void handleSearch();

    void stopSearch();

    void copySelectedText();                    //复制选中文字

    void highlightSelectedText();                   //高亮选中文字

    void addSelectedTextHightlightAnnotation();       //对选中文字添加高亮注释

    bool getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio);

    void defaultFocus();

    QList<deepin_reader::Annotation *> annotations();

    bool removeAnnotation(deepin_reader::Annotation *annotation);

    QList<qreal> scaleFactorList();

    qreal maxScaleFactor();

    QString filter();

    QSet<int> getBookMarkList() const;

    SheetOperation operation();

    Dr::FileType type();

    QString filePath();

    bool hasBookMark(int index);

    void zoomin();  //放大一级

    void zoomout();

    void setSidebarVisible(bool isVisible);

    void handleOpenSuccess();

    void showTips(const QString &tips, int iconIndex = 0);

    void openSlide();

    void closeSlide();

    bool slideOpened();

    void openFullScreen();

    void handleSlideKeyPressEvent(const QString &sKey);

//===========
public:
    //virtual int label2pagenum(QString label);

    //virtual bool haslabel();

    //virtual void docBasicInfo(stFileInfo &info);

    //virtual void getAllAnnotation(QList<stHighlightContent> &listres);

    //virtual QString pagenum2label(int index);

    //virtual QString addIconAnnotation(const QPoint &pos, const QColor &color = Qt::yellow, TextAnnoteType_Em type = TextAnnoteType_Note);

signals:
    void sigFindOperation(const int &);
//==============

signals:
    void sigFileChanged(Sheet *);    //被修改了 书签 笔记

    void sigOpened(Sheet *, bool);

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
    SheetOperation  m_operation;
    QSet<int>       m_bookmarks;

    SheetSidebar *m_sidebar = nullptr;
    SheetBrowser *m_browser = nullptr;

    QString         m_filePath;
    Dr::FileType    m_type;
    QString         m_uuid;

    bool m_fileChanged = false;
};

#endif // Sheet_H
