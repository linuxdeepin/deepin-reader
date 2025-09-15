// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BrowserMenu.h"
#include "ColorWidgetAction.h"
#include "DocSheet.h"
#include "Utils.h"
#include "Global.h"

#include <DFontSizeManager>
#include <QDebug>

BrowserMenu::BrowserMenu(QWidget *parent) : DMenu(parent)
{
    qDebug() << "BrowserMenu created";
    DFontSizeManager::instance()->bind(this, DFontSizeManager::T6);
    qDebug() << "BrowserMenu::BrowserMenu() - Constructor completed";
}

void BrowserMenu::initActions(DocSheet *sheet, int index, SheetMenuType_e type, const QString &copytext)
{
    qDebug() << "BrowserMenu::initActions type:" << type << "index:" << index << "fileType:" << sheet->fileType();
    m_type = type;
    m_pColorWidgetAction = nullptr;
    if (type == DOC_MENU_ANNO_ICON) {
        qDebug() << "BrowserMenu::initActions() - Processing DOC_MENU_ANNO_ICON";
        createAction(tr("Copy"), "CopyAnnoText");
        this->addSeparator();

        if (sheet->fileType() == Dr::FileType::PDF || sheet->fileType() == Dr::FileType::DOCX) {
            qDebug() << "BrowserMenu::initActions() - Adding PDF/DOCX annotation actions";
            createAction(tr("Remove annotation"), "RemoveAnnotation");
            createAction(tr("Add annotation"), "AddAnnotationIcon");
        }
        if (sheet->hasBookMark(index)) {
            qDebug() << "BrowserMenu::initActions() - Adding remove bookmark action";
            createAction(tr("Remove bookmark"), "RemoveBookmark");
        } else {
            qDebug() << "BrowserMenu::initActions() - Adding add bookmark action";
            createAction(tr("Add bookmark"), "AddBookmark");
        }
    } else if (type == DOC_MENU_ANNO_HIGHLIGHT || type == DOC_MENU_SELECT_TEXT) {
        qDebug() << "BrowserMenu::initActions() - Processing DOC_MENU_ANNO_HIGHLIGHT or DOC_MENU_SELECT_TEXT";
        if (type == DOC_MENU_ANNO_HIGHLIGHT) {
            qDebug() << "BrowserMenu::initActions() - Creating copy annotation action";
            QAction *copyAnnoAction = createAction(tr("Copy"), "CopyAnnoText");
            if (copytext.isEmpty()) {
                qDebug() << "BrowserMenu::initActions() - Disabling copy action due to empty text";
                copyAnnoAction->setDisabled(true);
            }
        } else {
            qDebug() << "BrowserMenu::initActions() - Creating copy action";
            createAction(tr("Copy"), "Copy");
        }
        this->addSeparator();

        QAction *highAct = createAction(tr("Highlight"), type != DOC_MENU_SELECT_TEXT ? "ChangeAnnotationColor" : "AddTextHighlight");
        if (type == DOC_MENU_ANNO_HIGHLIGHT) {
            qDebug() << "BrowserMenu::initActions() - Disabling highlight action for annotation highlight";
            highAct->setDisabled(true);
        }

        m_pColorWidgetAction = new ColorWidgetAction(this);
        connect(m_pColorWidgetAction, SIGNAL(sigBtnGroupClicked()), this, SLOT(onSetHighLight()));
        this->addAction(m_pColorWidgetAction);

        if (sheet->fileType() == Dr::FileType::PDF || sheet->fileType() == Dr::FileType::DOCX) {
            qDebug() << "BrowserMenu::initActions() - Adding PDF/DOCX highlight actions";
            QAction *rmHighAct = createAction(tr("Remove highlight"), "RemoveHighlight");
            rmHighAct->setDisabled(true);
            if (type == DOC_MENU_ANNO_HIGHLIGHT) {
                qDebug() << "BrowserMenu::initActions() - Enabling remove highlight for annotation highlight";
                rmHighAct->setDisabled(false);
            }
            this->addSeparator();
            createAction(tr("Add annotation"), "AddAnnotationHighlight");
        }

        if (sheet->hasBookMark(index)) {
            qDebug() << "BrowserMenu::initActions() - Adding remove bookmark action";
            createAction(tr("Remove bookmark"), "RemoveBookmark");
        } else {
            qDebug() << "BrowserMenu::initActions() - Adding add bookmark action";
            createAction(tr("Add bookmark"), "AddBookmark");
        }
    } else if (type == DOC_MENU_KEY) {
        qDebug() << "BrowserMenu::initActions() - Processing DOC_MENU_KEY";
        createAction(tr("Search"), "Search");
        this->addSeparator();

        if (sheet->hasBookMark(index)) {
            qDebug() << "BrowserMenu::initActions() - Adding remove bookmark action";
            createAction(tr("Remove bookmark"), "RemoveBookmark");
        } else {
            qDebug() << "BrowserMenu::initActions() - Adding add bookmark action";
            createAction(tr("Add bookmark"), "AddBookmark");
        }

        this->addSeparator();

        if (sheet->isFullScreen()) {
            qDebug() << "BrowserMenu::initActions() - Adding exit fullscreen action";
            createAction(tr("Exit fullscreen"), "ExitFullscreen");
        } else {
            qDebug() << "BrowserMenu::initActions() - Adding fullscreen action";
            createAction(tr("Fullscreen"), "Fullscreen");
        }

        createAction(tr("Slide show"), "SlideShow");
        this->addSeparator();

        QAction *pFirstPage = createAction(tr("First page"), "FirstPage");
        QAction *pPrevPage = createAction(tr("Previous page"), "PreviousPage");
        if (sheet->currentIndex() == 0) {
            qDebug() << "BrowserMenu::initActions() - Disabling first/previous page actions (at first page)";
            pFirstPage->setDisabled(true);
            pPrevPage->setDisabled(true);
        }

        QAction *pNextPage = createAction(tr("Next page"), "NextPage");
        QAction *pEndPage = createAction(tr("Last page"), "LastPage");
        if (sheet->currentIndex() == sheet->pageCount() - 1) {
            qDebug() << "BrowserMenu::initActions() - Disabling next/last page actions (at last page)";
            pNextPage->setDisabled(true);
            pEndPage->setDisabled(true);
        }

        this->addSeparator();

        createAction(tr("Rotate left"), "RotateLeft");
        createAction(tr("Rotate right"), "RotateRight");
        this->addSeparator();

        createAction(tr("Print"), "Print");
        createAction(tr("Document info"), "DocumentInfo");
    } else {
        qDebug() << "BrowserMenu::initActions() - Processing default menu type";
        if (sheet->fileType() == Dr::FileType::PDF || sheet->fileType() == Dr::FileType::DOCX) {
            qDebug() << "BrowserMenu::initActions() - Adding search action for PDF/DOCX";
            createAction(tr("Search"), "Search");
            this->addSeparator();
        }

        if (sheet->hasBookMark(index)) {
            qDebug() << "BrowserMenu::initActions() - Adding remove bookmark action";
            createAction(tr("Remove bookmark"), "RemoveBookmark");
        } else {
            qDebug() << "BrowserMenu::initActions() - Adding add bookmark action";
            createAction(tr("Add bookmark"), "AddBookmark");
        }

        if (sheet->fileType() == Dr::FileType::PDF || sheet->fileType() == Dr::FileType::DOCX) {
            qDebug() << "BrowserMenu::initActions() - Adding annotation action for PDF/DOCX";
            createAction(tr("Add annotation"), "AddAnnotationIcon");
        }

        this->addSeparator();

        if (sheet->isFullScreen()) {
            qDebug() << "BrowserMenu::initActions() - Adding exit fullscreen action";
            createAction(tr("Exit fullscreen"), "ExitFullscreen");
        } else {
            qDebug() << "BrowserMenu::initActions() - Adding fullscreen action";
            createAction(tr("Fullscreen"), "Fullscreen");
        }

        createAction(tr("Slide show"), "SlideShow");
        this->addSeparator();

        QAction *pFirstPage = createAction(tr("First page"), "FirstPage");
        QAction *pPrevPage = createAction(tr("Previous page"), "PreviousPage");
        if (sheet->currentIndex() == 0) {
            qDebug() << "BrowserMenu::initActions() - Disabling first/previous page actions (at first page)";
            pFirstPage->setDisabled(true);
            pPrevPage->setDisabled(true);
        }

        QAction *pNextPage = createAction(tr("Next page"), "NextPage");
        QAction *pEndPage = createAction(tr("Last page"), "LastPage");
        if (sheet->currentIndex() == sheet->pageCount() - 1) {
            qDebug() << "BrowserMenu::initActions() - Disabling next/last page actions (at last page)";
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
    qDebug() << "BrowserMenu::initActions() - Init actions completed";
}

void BrowserMenu::hideEvent(QHideEvent *event)
{
    // qDebug() << "BrowserMenu hidden";
    emit sigMenuHide();

    DMenu::hideEvent(event);
    // qDebug() << "BrowserMenu::hideEvent() - Hide event completed";
}

QAction *BrowserMenu::createAction(const QString &displayname, const QString &objectname)
{
    // qDebug() << "BrowserMenu::createAction:" << objectname;
    QAction *action = new  QAction(displayname, this);
    action->setObjectName(objectname);
    connect(action, SIGNAL(triggered()), this, SLOT(onItemClicked()));
    this->addAction(action);
    // qDebug() << "BrowserMenu::createAction() - Action created and added to menu";
    return action;
}

void BrowserMenu::onItemClicked()
{
    QString actionName = sender()->objectName();
    qInfo() << "BrowserMenu item clicked:" << actionName;
    emit signalMenuItemClicked(actionName);
    qDebug() << "BrowserMenu::onItemClicked() - Emitted signalMenuItemClicked with action:" << actionName;
}

void BrowserMenu::onSetHighLight()
{
    // qDebug() << "BrowserMenu::onSetHighLight type:" << m_type;
    if (m_type == DOC_MENU_SELECT_TEXT) {
        // qDebug() << "BrowserMenu::onSetHighLight() - Emitting AddTextHighlight signal";
        emit signalMenuItemClicked("AddTextHighlight");
    } else if (m_type == DOC_MENU_ANNO_HIGHLIGHT) {
        // qDebug() << "BrowserMenu::onSetHighLight() - Emitting ChangeAnnotationColor signal";
        emit signalMenuItemClicked("ChangeAnnotationColor");
    }
    this->close();
    // qDebug() << "BrowserMenu::onSetHighLight() - Set highlight handler completed";
}
