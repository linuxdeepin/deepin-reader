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
#ifndef MAINSPLITTER_H
#define MAINSPLITTER_H

#include <DSplitter>
#include "FileDataModel.h"

DWIDGET_USE_NAMESPACE

class DocShowShellWidget;
class LeftSidebarWidget;
class MainSplitterPrivate;

class MainSplitter : public DSplitter
{
    Q_OBJECT
    Q_DISABLE_COPY(MainSplitter)

public:
    explicit MainSplitter(DWidget *parent = nullptr);
    ~MainSplitter() override;

public:
    QString qGetPath() const;
    void qSetPath(const QString &strPath);

    void qSetFileChange(const int &);
    int qGetFileChange();

    void saveData();

    FileDataModel qGetFileData() const;
    void setFileData(const FileDataModel &) const;

    void OnOpenSliderShow();
    void OnExitSliderShow();

private:
    void InitWidget();

private slots:
    void SlotOpenFileOk();
    void SlotNotifyMsg(const int &, const QString &);

private:
    LeftSidebarWidget   *m_pLeftWidget = nullptr;
    bool                m_bOldState = false;

    DocShowShellWidget  *m_pDocWidget   = nullptr;

    MainSplitterPrivate     *d_ptr = nullptr;
};

#endif // MAINSPLITTER_H
