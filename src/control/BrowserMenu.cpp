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
#include "BrowserMenu.h"
#include "widgets/ColorWidgetAction.h"
#include "Sheet.h"

#include <DFontSizeManager>

BrowserMenu::BrowserMenu(QWidget *parent) : DMenu(parent)
{
    DFontSizeManager::instance()->bind(this, DFontSizeManager::T6);
}

void BrowserMenu::initActions(Sheet *Sheet, int index, SheetMenuType_e type)
{
    if (type == DOC_MENU_ANNO_ICON) {
        createAction(tr("Copy"), "Copy");
        this->addSeparator();
        createAction(tr("Remove annotation"), "RemoveAnnotation");
        createAction(tr("Add annotation"), "AddAnnotationIcon");
        createAction(tr("Add bookmark"), "AddBookmark");
    } else if (type == DOC_MENU_ANNO_HIGHLIGHT || type == DOC_MENU_SELECT_TEXT) {
        createAction(tr("Copy"), "Copy");
        this->addSeparator();
        ColorWidgetAction *pColorWidgetAction = new ColorWidgetAction(this);
        pColorWidgetAction->setObjectName("ColorWidgetAction");
        connect(pColorWidgetAction, SIGNAL(sigBtnGroupClicked(const int &)), this, SLOT(onSetHighLight(const int &)));
        this->addAction(pColorWidgetAction);

        QAction *rmHighAct = createAction(tr("Remove highlight"), "RemoveHighlight");
        rmHighAct->setDisabled(true);
        if (type == DOC_MENU_ANNO_HIGHLIGHT) rmHighAct->setDisabled(false);
        this->addSeparator();
        createAction(tr("Add annotation"), "AddAnnotationHighlight");
        createAction(tr("Add bookmark"), "AddBookmark");
    } else {
        createAction(tr("Search"), "Search");
        this->addSeparator();

        if (Sheet->hasBookMark(index))
            createAction(tr("Remove bookmark"), "RemoveBookmark");
        else
            createAction(tr("Add bookmark"), "AddBookmark");

        createAction(tr("Add annotation"), "AddAnnotationIcon");
        this->addSeparator();

        createAction(tr("Fullscreen"), "Fullscreen");
        createAction(tr("Slide show"), "SlideShow");
        this->addSeparator();

        QAction *pFirstPage = createAction(tr("First page"), "FirstPage");
        QAction *pPrevPage = createAction(tr("Previous page"), "PreviousPage");
        if (index == 0) {
            pFirstPage->setDisabled(true);
            pPrevPage->setDisabled(true);
        }

        QAction *pNextPage = createAction(tr("Next page"), "NextPage");
        QAction *pEndPage = createAction(tr("Last page"), "LastPage");
        if (index == Sheet->pagesNumber() - 1) {
            pNextPage->setDisabled(true);
            pEndPage->setDisabled(true);
        }

        this->addSeparator();

        createAction(tr("Rotate left"), "RotateLeft");
        createAction(tr("Rotate right"), "RotateRight");
        this->addSeparator();

        createAction(tr("Print"), "Print");
        createAction(tr("Document info"), "DocumentInfo");
    }
}

QAction *BrowserMenu::createAction(const QString &displayname, const QString &objectname)
{
    QAction *action = new  QAction(displayname, this);
    action->setObjectName(objectname);
    connect(action, SIGNAL(triggered()), this, SLOT(onItemClicked()));
    this->addAction(action);
    return action;
}

void BrowserMenu::onItemClicked()
{
    emit signalMenuItemClicked(sender()->objectName());
}

void BrowserMenu::onSetHighLight(const int &colorIndex)
{
    emit signalMenuItemClicked(sender()->objectName(), colorIndex);
    this->close();
}
