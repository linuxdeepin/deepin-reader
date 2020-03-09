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
#ifndef SCALEWIDGET_H
#define SCALEWIDGET_H

#include "CustomControl/CustomWidget.h"

class ScaleWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ScaleWidget)
public:
    explicit ScaleWidget(DWidget *parent = nullptr);
    ~ScaleWidget() override;

signals:
    void sigScaleChanged();

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

    // CustomWidget interface
protected:
    void initWidget() override;

private slots:
    void slotPrevScale();
    void slotNextScale();
    void SlotCurrentTextChanged(const QString &);

    void SlotReturnPressed();

private:
    void SetComboBoxMax(const QString &);
    void SetFitScale(const QString &);
    void onShortKey(const QString &keyType);

private:
    DComboBox *scaleComboBox     = nullptr;
    int         m_nCurrentIndex = -1;           //  当前选项
    int         m_nMaxScale = 0;

    QList<int> dataList;
};

#endif // SCALEWIDGET_H
