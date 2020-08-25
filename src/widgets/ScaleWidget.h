/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
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

#include <DWidget>
#include <QPointer>
#include <DLineEdit>

#include "DocSheet.h"
#include "ScaleMenu.h"
#include <DIconButton>

class DocSheet;
class ScaleMenu;
class ScaleWidget : public Dtk::Widget::DWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ScaleWidget)
public:
    explicit ScaleWidget(Dtk::Widget::DWidget *parent = nullptr);
    ~ScaleWidget();

    void setSheet(DocSheet *sheet);
    void clear();

    DIconButton *getDecreaseBtn();

private:
    void initWidget();

public slots:
    void onPrevScale();
    void onNextScale();

private slots:
    void onReturnPressed();
    void onEditFinished();
    void onArrowBtnlicked();

private:
    QPointer<DocSheet> m_sheet;
    Dtk::Widget::DLineEdit *m_lineEdit;
    ScaleMenu *m_ScaleMenu = nullptr;
};

#endif // SCALEWIDGET_H
