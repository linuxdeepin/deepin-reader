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
#ifndef TITLEMENU_H
#define TITLEMENU_H

#include "widgets/CustomMenu.h"

class QSignalMapper;
class DocSheet;
class HandleMenu;
class TitleMenu : public CustomMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(TitleMenu)

public:
    explicit TitleMenu(DWidget *parent = nullptr);

signals:
    void sigActionTriggered(const QString &action);

public slots:
    void onCurSheetChanged(DocSheet *);

public:
    void disableAllAction();

private:
    void disableSaveButton(bool disable);

protected:
    void initActions() override;

private:
    void createActionMap(QSignalMapper *pSigManager, const QStringList &actionList, const QStringList &actionObjList);

    QAction *createAction(const QString &actionName, const QString &);

private:
    HandleMenu *m_handleMenu;
};


#endif // TITLEMENU_H
