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
#include <QPointer>

class DocSheet;
class ScaleWidget : public DWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ScaleWidget)
public:
    explicit ScaleWidget(DWidget *parent = nullptr);

    ~ScaleWidget() override;

    void setSheet(DocSheet *sheet);

    void clearComboBox();

    void paintEvent(QPaintEvent *event);

protected:
    void initWidget();

private slots:
    void slotPrevScale();

    void slotNextScale();

    void SlotCurrentTextChanged(const QString &);

    void SlotReturnPressed();

    void onEditFinished();

private:
    DComboBox  *m_scaleComboBox     = nullptr;
    int         m_nCurrentIndex = -1;           //  当前选项
    int         m_nMaxScale = 1000;

    QList<double> dataList;
    QPointer<DocSheet> m_sheet;
};

#endif // SCALEWIDGET_H
