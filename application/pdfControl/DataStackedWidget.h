/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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
#ifndef DATASTACKEDWIDGET_H
#define DATASTACKEDWIDGET_H

#include <DStackedWidget>

DWIDGET_USE_NAMESPACE

class BookMarkWidget;
class CatalogWidget;
class NotesWidget;
class SearchResWidget;
class ThumbnailWidget;
class DocSheet;
const double EPSINON = 0.00000001;

class DataStackedWidget : public DStackedWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DataStackedWidget)

public:
    explicit DataStackedWidget(DocSheet *sheet, DWidget *parent = nullptr);

    void handleOpenSuccess();

    void handleRotate(int rotate);

    void handlePage(int page);

    void handleBookMark(int page, int state);

signals:
    void sigAnntationMsg(const int &, const QString &);

    void sigDeleteAnntation(const int &, const QString &);

    void sigFitChanged(int state);

public:
    void SetFindOperation(const int &);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void slotSetStackCurIndex(const int &);

    void slotAdaptWindowSize(const double &scale); //缩略图列表自适应窗体大小  add by duanxiaohui 2020-3-20

    void slotUpdateThumbnail(const int &page);//添加或者移除高亮，要更新列表中相应的缩略图 add by duanxiaohui 2020-3-26

private:
    void InitWidgets();

    void DealWithPressKey(const QString &sKey);

    void onJumpToPrevPage();

    void onJumpToNextPage();

    void DeleteItemByKey();

private:
    ThumbnailWidget     *m_pThWidget = nullptr;
    CatalogWidget       *m_pCatalogWidget = nullptr;
    BookMarkWidget      *m_pBookMarkWidget = nullptr;
    NotesWidget         *m_pNotesWidget = nullptr;
    SearchResWidget     *m_pSearchResWidget = nullptr;

    QString             m_strBindPath = "";
    DocSheet    *m_sheet = nullptr;
    double m_dScale{1.0};

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
};

#endif // DATASTACKEDWIDGET_H
