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
#ifndef TITLEMENU_H
#define TITLEMENU_H

#include "CustomControl/CustomMenu.h"

class QSignalMapper;
class DocSheet;
class TitleMenu : public CustomMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(TitleMenu)

public:
    explicit TitleMenu(DWidget *parent = nullptr);

private:
    static TitleMenu *g_onlyTitleMenu;

signals:
    void sigActionTriggered(const QString &action);

public:
    static TitleMenu *Instance(DWidget *parent = nullptr);

public slots:
    void onCurSheetChanged(DocSheet *);

    void onCurSheetSaved(DocSheet *);

public:
    int dealWithData(const int &, const QString &) override;

    void flushSaveButton();

    void disableAllAction();

private:
    void disableSaveButton(bool disable);

protected:
    void initActions() override;

private:
    void __CreateActionMap(QSignalMapper *pSigManager, const QStringList &actionList, const QStringList &actionObjList);

    QAction *__CreateAction(const QString &actionName, const QString &);

};


#endif // TITLEMENU_H
