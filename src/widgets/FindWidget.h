/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     duanxiaohui<duanxiaohui@uniontech.com>
 *
 * Maintainer: duanxiaohui<duanxiaohui@uniontech.com>
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
#ifndef FINDWIDGET_H
#define FINDWIDGET_H

#include "Application.h"

#include <DWidget>
#include <DFloatingWidget>
#include <DSearchEdit>

DWIDGET_USE_NAMESPACE

class DocSheet;
class FindWidget : public DFloatingWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(FindWidget)

public:
    explicit FindWidget(DWidget *parent = nullptr);

    ~FindWidget() override;

public:
    void setDocSheet(DocSheet *sheet);
    void showPosition(const int &);
    void setSearchEditFocus();
    void stopSearch();
    void setEditAlert(const int &iFlag);

private slots:
    void findCancel();

    void handleContentChanged();

    void slotFindNextBtnClicked();

    void slotFindPrevBtnClicked();

    void slotEditAborted();

    void slotClearContent();

private:
    void initWidget();

private:
    DocSheet *m_docSheet = nullptr;
    DSearchEdit *m_pSearchEdit = nullptr;
    QString m_strLastFindText = ""; //上一次查找内容
};

#endif
