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
#ifndef DocSheetOld_H
#define DocSheetOld_H

#include <DSplitter>
#include <QMap>

#include "Global.h"
#include "pdfControl/docview/CommonStruct.h"
#include "ModuleHeader.h"
#include "document/Model.h"

class SheetSidebar;
class SlideWidget;
struct DocOperation {
    Dr::LayoutMode layoutMode   = Dr::SinglePageMode;
    Dr::MouseShape mouseShape   = Dr::MouseShapeNormal;
    Dr::ScaleMode scaleMode     = Dr::ScaleFactorMode;
    Dr::Rotation rotation       = Dr::RotateBy0;
    qreal scaleFactor           = 1.0;
    bool sidebarVisible         = false;
    int  sidebarIndex           = 0;
    int  currentPage            = 1;
};

class DocSheetOld : public Dtk::Widget::DSplitter
{
    Q_OBJECT
    Q_DISABLE_COPY(DocSheetOld)
    friend class Database;
    friend class SheetSidebar;
public:
    explicit DocSheetOld(Dr::FileType type, QString filePath, Dtk::Widget::DWidget *parent = nullptr);

    virtual ~DocSheetOld();

    virtual void initOperationData(const DocOperation &opera);

    virtual bool isOpen();

    virtual void openFile();

    virtual bool openFileExec();

    virtual int pagesNumber();

    virtual int currentPage();      //从1开始

    virtual int currentIndex();     //从0开始

    virtual void jumpToIndex(int index);

    virtual void jumpToPage(int page);

    virtual void jumpToFirstPage();

    virtual void jumpToLastPage();

    virtual void jumpToNextPage();

    virtual void jumpToPrevPage();

    virtual Outline outline();

    virtual void jumpToOutline(const qreal  &realleft, const qreal &realtop, unsigned int ipage = 0);

    virtual void jumpToHighLight(const QString &uuid, int ipage);

    virtual void rotateLeft();

    virtual void rotateRight();

    virtual void setBookMark(int index, int state);

    virtual void setBookMarks(const QList<BookMarkStatus_t> &);

    virtual void setLayoutMode(Dr::LayoutMode mode);

    virtual void setScaleMode(Dr::ScaleMode mode);

    virtual void setScaleFactor(qreal scaleFactor);     //base is 1 ;设置后自动取消自适应

    virtual void setMouseShape(Dr::MouseShape shape);

    virtual void openMagnifier();

    virtual void closeMagnifier();

    virtual bool magnifierOpened();

    virtual bool fileChanged();

    virtual bool saveData();

    virtual bool saveAsData(QString filePath);

    virtual void handleSearch();

    virtual void stopSearch();

    virtual void copySelectedText();                    //复制选中文字

    virtual void highlightSelectedText();                   //高亮选中文字

    virtual void addSelectedTextHightlightAnnotation();       //对选中文字添加高亮注释

    virtual bool getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio);

    virtual QString filter();

    virtual void defaultFocus();

    virtual void deleteAnnotation(const int &msgType, const QString &msgContent);

    virtual void deleteAnnotations(const QList<AnnotationInfo_t> &);

    virtual QList<qreal> scaleFactorList();

    virtual qreal maxScaleFactor();

    virtual void setActive(const bool &);

    virtual QList< deepin_reader::Annotation * > annotations();

    QSet<int> getBookMarkList() const;

    DocOperation operation();

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

protected:
    DocOperation  m_operation;
    QSet<int>     m_bookmarks;

    SheetSidebar *m_sidebar = nullptr;

signals:
    void sigFileChanged(DocSheetOld *);    //被修改了 书签 笔记

    void sigOpened(DocSheetOld *, bool);

private:
    QString         m_filePath;

    Dr::FileType    m_type;

    QString         m_uuid;

public:
    static bool existFileChanged();

    static QUuid getUuid(DocSheetOld *);

    static DocSheetOld *getSheet(QString uuid);

    static QMap<QString, DocSheetOld *> g_map;

    //===========以上是改版后的,优先使用(pdf看情况，如果未实现则不用) ,以下则逐步替换和删除


public:
    virtual int label2pagenum(QString label);

    virtual bool haslabel();

    virtual void docBasicInfo(stFileInfo &info);

    virtual void getAllAnnotation(QList<stHighlightContent> &listres);

    virtual QString pagenum2label(int index);

    virtual QString addIconAnnotation(const QPoint &pos, const QColor &color = Qt::yellow, TextAnnoteType_Em type = TextAnnoteType_Note);

signals:
    void sigFindOperation(const int &);

};

#endif // DocSheetOld_H
