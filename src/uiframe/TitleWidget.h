/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     wangzhixuan<wangzhixuan@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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

#include "CustomControl/CustomWidget.h"
#include "pdfControl/docview/commonstruct.h"

//class ScaleMenu;
class FontMenu;
class HandleMenu;
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
    void setMagnifierState();

    void setControlEnabled(const bool &enable);

protected:
    void initWidget() override;

public slots:
    void onCurSheetChanged(DocSheet *);

private slots:
    void slotUpdateTheme();

    void SlotSetCurrentTool(const int &);

    void slotFindOperation(const int &);

    void on_thumbnailBtn_clicked();

    void on_settingBtn_clicked();

    void on_handleShapeBtn_clicked();

    void on_searchBtn_clicked();

private:
    void initBtns();

    void __InitHandel();

    void __InitSelectTool();

    void setDefaultShape();

    void setHandleShape();

    DPushButton *createBtn(const QString &btnName, bool bCheckable = false);

    void SetBtnDisable(const bool &bAble);

private:
    QStringList shortKeyList;

    HandleMenu *m_pHandleMenu = nullptr;
    FontMenu *m_pFontMenu = nullptr;

    DPushButton *m_pThumbnailBtn = nullptr;
    DPushButton *m_pSettingBtn = nullptr;
    DPushButton *m_pHandleShapeBtn = nullptr;

    ScaleWidget     *m_pSw = nullptr;
    DIconButton     *m_pSearchBtn = nullptr;

    QPointer<DocSheet> m_curSheet = nullptr;
};

#endif  // TITLEWIDGET_H
