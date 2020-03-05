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
#include "DataStackedWidget.h"

#include <QJsonObject>
#include <QJsonDocument>

#include "thumbnail/ThumbnailWidget.h"
#include "catalog/CatalogWidget.h"
#include "bookmark/BookMarkWidget.h"
#include "note/NotesWidget.h"
#include "search/BufferWidget.h"
#include "search/SearchResWidget.h"

#include "widgets/main/MainTabWidgetEx.h"

DataStackedWidget::DataStackedWidget(DWidget *parent)
    : DStackedWidget(parent)
{
    InitWidgets();
}

int DataStackedWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        FileDataModel fdm = MainTabWidgetEx::Instance()->qGetFileData(msgContent);
        int nId = fdm.qGetData(LeftIndex);
        if (nId == -1) {
            nId = 0;
        }
        setCurrentIndex(nId);
    }

    int nRes = m_pThWidget->dealWithData(msgType, msgContent);
    if (nRes != MSG_OK) {
        nRes = m_pCatalogWidget->dealWithData(msgType, msgContent);
        if (nRes != MSG_OK) {
            nRes = m_pBookMarkWidget ->dealWithData(msgType, msgContent);
            if (nRes != MSG_OK) {
                nRes = m_pNotesWidget->dealWithData(msgType, msgContent);
                if (nRes == MSG_OK)
                    return MSG_OK;
            }
        }
    }

    return MSG_NO_OK;
}

void DataStackedWidget::keyPressEvent(QKeyEvent *event)
{
    QStringList pFilterList = QStringList() << KeyStr::g_pgup << KeyStr::g_pgdown
                              << KeyStr::g_down << KeyStr::g_up
                              << KeyStr::g_left << KeyStr::g_right
                              << KeyStr::g_del;
    QString key = Utils::getKeyshortcut(event);
    if (pFilterList.contains(key)) {
        DealWithPressKey(key);
    }

    DStackedWidget::keyPressEvent(event);
}

void DataStackedWidget::slotSetStackCurIndex(const int &iIndex)
{
    setCurrentIndex(iIndex);

    //  前一个是 出来搜索结果了, 后一个是正在搜索, 两个都不需要保存在记录中
    if (iIndex == WIDGET_SEARCH || iIndex == WIDGET_BUFFER) {
        emit sigSearchWidgetState(iIndex);
    } else {
        QJsonObject obj;
        obj.insert("content", QString::number(iIndex));
        obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET);

        QJsonDocument doc(obj);

        dApp->m_pModelService->notifyMsg(MSG_LEFTBAR_STATE,  doc.toJson(QJsonDocument::Compact));
    }
}

void DataStackedWidget::InitWidgets()
{
    m_pThWidget = new ThumbnailWidget(this);
    insertWidget(WIDGET_THUMBNAIL, m_pThWidget);

    m_pCatalogWidget = new CatalogWidget(this);
    insertWidget(WIDGET_catalog, m_pCatalogWidget);

    m_pBookMarkWidget = new BookMarkWidget(this);

    insertWidget(WIDGET_BOOKMARK, m_pBookMarkWidget);

    m_pNotesWidget = new NotesWidget(this);
    connect(m_pBookMarkWidget, SIGNAL(sigSetBookMarkState(const int &, const int &)),
            m_pThWidget, SLOT(SlotSetBookMarkState(const int &, const int &)));

    insertWidget(WIDGET_NOTE, m_pNotesWidget);
//    insertWidget(WIDGET_SEARCH, new SearchResWidget(this));

//    auto buffWidget = new BufferWidget(this);
//    connect(this, SIGNAL(sigSearchWidgetState(const int &)), buffWidget, SLOT(SlotSetSpinnerState(const int &)));

//    insertWidget(WIDGET_BUFFER, buffWidget);
    setCurrentIndex(WIDGET_THUMBNAIL);
}

void DataStackedWidget::DealWithPressKey(const QString &sKey)
{
    if (sKey == KeyStr::g_up || sKey == KeyStr::g_pgup || sKey == KeyStr::g_left) {
        onJumpToPrevPage();
    } else if (sKey == KeyStr::g_down || sKey == KeyStr::g_pgdown || sKey == KeyStr::g_right) {
        onJumpToNextPage();
    } else if (sKey == KeyStr::g_del) {
        DeleteItemByKey();
    }
}

void DataStackedWidget::onJumpToPrevPage()
{
    int iIndex = this->currentIndex();
    if (iIndex == WIDGET_THUMBNAIL) {
        auto widget = this->findChild<ThumbnailWidget *>();
        if (widget) {
            widget->prevPage();
        }
    }  else if (iIndex == WIDGET_BOOKMARK) {
        auto widget = this->findChild<BookMarkWidget *>();
        if (widget) {
            widget->prevPage();
        }
    } else if (iIndex == WIDGET_NOTE) {
        auto widget = this->findChild<NotesWidget *>();
        if (widget) {
            widget->prevPage();
        }
    }
}

void DataStackedWidget::onJumpToNextPage()
{
    int iIndex = this->currentIndex();
    if (iIndex == WIDGET_THUMBNAIL) {
        auto widget = this->findChild<ThumbnailWidget *>();
        if (widget) {
            widget->nextPage();
        }
    }  else if (iIndex == WIDGET_BOOKMARK) {
        auto widget = this->findChild<BookMarkWidget *>();
        if (widget) {
            widget->nextPage();
        }
    } else if (iIndex == WIDGET_NOTE) {
        auto widget = this->findChild<NotesWidget *>();
        if (widget) {
            widget->nextPage();
        }
    }
}

void DataStackedWidget::DeleteItemByKey()
{
    int iIndex = this->currentIndex();
    if (iIndex == WIDGET_BOOKMARK) {
        auto widget = this->findChild<BookMarkWidget *>();
        if (widget) {
            widget->DeleteItemByKey();
        }
    } else if (iIndex == WIDGET_NOTE) {
        auto widget = this->findChild<NotesWidget *>();
        if (widget) {
            widget->DeleteItemByKey();
        }
    }
}

