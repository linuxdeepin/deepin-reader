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

class DataStackedWidget : public DStackedWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DataStackedWidget)

public:
    explicit DataStackedWidget(DWidget *parent = nullptr);

signals:
    void sigAnntationMsg(const int &, const QString &);
    void sigBookMarkMsg(const int &, const QString &);
    void sigDeleteAnntation(const int &, const QString &);
    void sigUpdateThumbnail(const int &);

public:
    int dealWithData(const int &, const QString &);
    void SetFindOperation(const int &);

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;

    void notifyMsg(const int &, const QString &);
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

    void OnOpenFileOk(const QString &);

private:
    ThumbnailWidget     *m_pThWidget = nullptr;
    CatalogWidget       *m_pCatalogWidget = nullptr;
    BookMarkWidget      *m_pBookMarkWidget = nullptr;
    NotesWidget         *m_pNotesWidget = nullptr;
    SearchResWidget     *m_pSearchResWidget = nullptr;

    QString             m_strBindPath = "";
};

#endif // DATASTACKEDWIDGET_H
