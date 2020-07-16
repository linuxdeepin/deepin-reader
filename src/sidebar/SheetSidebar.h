/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     leiyu
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

#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H

#include <QResizeEvent>
#include <DToolButton>
#include <QStackedLayout>

#include "widgets/CustomWidget.h"

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
class SheetSidebar : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SheetSidebar)

signals:
    void sigAdaptWindowSize(const double &scale);

public :
    explicit SheetSidebar(DocSheet *parent = nullptr, PreviewWidgesFlags widgesFlag = PREVIEW_THUMBNAIL | PREVIEW_CATALOG | PREVIEW_BOOKMARK | PREVIEW_NOTE);
    ~SheetSidebar() override;

    void setBookMark(int index, int state);
    void setCurrentPage(int page);
    void handleOpenSuccess();
    void handleFindOperation(int);
    void handleFindContentComming(const deepin_reader::SearchResult &);
    int  handleFindFinished();
    void handleRotate(int);
public slots:
    void handleUpdateThumbnail(const int &index);
    void handleAnntationMsg(const int &, deepin_reader::Annotation *anno);

protected:
    void initWidget() Q_DECL_OVERRIDE;
    void adaptWindowSize(const double &) Q_DECL_OVERRIDE;

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private slots:
    void onBtnClicked(int index);
    void onUpdateWidgetTheme();

private:
    void initConnections();
    DToolButton *createBtn(const QString &btnName, const QString &objName);
    void dealWithPressKey(const QString &sKey);
    void onJumpToPrevPage();
    void onJumpToNextPage();
    void deleteItemByKey();
    void handWidgetDocOpenSuccess();

private:
    qreal m_scale;
    int  m_searchId;
    bool m_bOldVisible;
    bool m_bOpenDocOpenSuccess;

    DocSheet *m_sheet;
    QStackedLayout *m_stackLayout;
    QButtonGroup *m_btnGroup;
    PreviewWidgesFlags m_widgetsFlag;

    ThumbnailWidget *m_thumbnailWidget;
    CatalogWidget   *m_catalogWidget;
    BookMarkWidget  *m_bookmarkWidget;
    NotesWidget     *m_notesWidget;
    SearchResWidget *m_searchWidget;
};
#endif  // LEFTSHOWWIDGET_H
