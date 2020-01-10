/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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
#ifndef BOOKMARKITEMWIDGET_H
#define BOOKMARKITEMWIDGET_H

#include <DLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include "CustomItemWidget.h"

/**
 * @brief The BookMarkWidget class
 * @brief   书签列表item
 */


class BookMarkItemWidget : public CustomItemWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BookMarkItemWidget)

public:
    explicit BookMarkItemWidget(QWidget *parent = nullptr);
    ~BookMarkItemWidget() Q_DECL_OVERRIDE;

signals:
    void sigDeleleteItem(const int &);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    bool bSelect();
    void setBSelect(const bool &paint);

private slots:
    void slotDltBookMark();
    void slotShowContextMenu(const QPoint &);

protected:
    void initWidget() Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_bPaint = false;                    // 是否绘制选中item
    DMenu *m_menu = nullptr;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
};

#endif // BOOKMARKITEMWIDGET_H
