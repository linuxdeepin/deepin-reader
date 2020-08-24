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
#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <DPushButton>
#include <DIconButton>
#include <QPointer>

#include "DocSheet.h"
#include "widgets/CustomWidget.h"
#include "widgets/CustemPushButton.h"

class ScaleWidget;
class DocSheet;
class TitleWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(TitleWidget)

public:
    explicit TitleWidget(DWidget *parent = nullptr);
    ~TitleWidget() override;

public:
    void setControlEnabled(const bool &enable);

protected:
    void initWidget() override;

public slots:
    void onCurSheetChanged(DocSheet *);

private slots:
    void onFindOperation(const int &);
    void onThumbnailBtnClicked();

private:
    void initBtns();
    void setBtnDisable(const bool &bAble);
    CustemPushButton *createBtn(const QString &btnName, bool bCheckable = false);

private:
    QStringList shortKeyList;

    ScaleWidget *m_pSw = nullptr;
    CustemPushButton *m_pThumbnailBtn = nullptr;
    QPointer<DocSheet> m_curSheet = nullptr;
};

#endif  // TITLEWIDGET_H
