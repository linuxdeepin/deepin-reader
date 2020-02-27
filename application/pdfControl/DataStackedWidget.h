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

class DataStackedWidget : public DStackedWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DataStackedWidget)

public:
    explicit DataStackedWidget(DWidget *parent = nullptr);

signals:
    void sigSearchWidgetState(const int &);

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private slots:
    void slotSetStackCurIndex(const int &);

private:
    void InitWidgets();
    void DealWithPressKey(const QString &sKey);
    void onJumpToPrevPage();
    void onJumpToNextPage();
    void DeleteItemByKey();
};

#endif // DATASTACKEDWIDGET_H
