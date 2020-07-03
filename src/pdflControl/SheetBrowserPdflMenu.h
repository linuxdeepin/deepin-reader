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
#ifndef SHEETBROWSERPDFLMENU_H
#define SHEETBROWSERPDFLMENU_H

#include <DMenu>

DWIDGET_USE_NAMESPACE
typedef enum DocSheetMenuType_e {
    DOC_MENU_DEFAULT = 0,
    DOC_MENU_ANNO_ICON = 1,
    DOC_MENU_ANNO_HIGHLIGHT = 2,
    DOC_MENU_SELECT_TEXT = 3
} DocSheetMenuType_e;

class DocSheet;
class SheetBrowserPDFLMenu : public DMenu
{
    Q_OBJECT
signals:
    void signalMenuItemClicked(const QString &objectname, const QVariant &param = QVariant());

public:
    explicit SheetBrowserPDFLMenu(QWidget *parent = nullptr);
    void initActions(DocSheet *docsheet, int index, DocSheetMenuType_e type);

private slots:
    void onItemClicked();
    void onSetHighLight(const int &colorIndex);

private:
    QAction *createAction(const QString &displayname, const QString &objectname);
};

#endif // SHEETBROWSERPDFLMENU_H
