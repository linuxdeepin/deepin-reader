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
#ifndef PDFCONTROL_CATALOGWIDGET_H
#define PDFCONTROL_CATALOGWIDGET_H

#include "CustomControl/CustomWidget.h"

#include <DLabel>

/**
 * @brief The CatalogWidget class
 * @brief       目录 界面
 */

class CatalogWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CatalogWidget)

public:
    explicit CatalogWidget(DWidget *parent = nullptr);
    ~CatalogWidget() Q_DECL_OVERRIDE;

signals:
    void sigDealWithData(const int &, const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    void initConnections();

private slots:
    void SlotDealWithData(const int &, const QString &);

private:
    void setCatalogTitle(const QString &);

private:
    DLabel  *titleLabel = nullptr;
};


#endif // CATALOGWIDGET_H
