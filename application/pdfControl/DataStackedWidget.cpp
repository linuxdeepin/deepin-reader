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

#include "thumbnail/ThumbnailWidget.h"
#include "catalog/CatalogWidget.h"
#include "bookmark/BookMarkWidget.h"
#include "note/NotesWidget.h"
#include "search/BufferWidget.h"
#include "search/SearchResWidget.h"

DataStackedWidget::DataStackedWidget(DWidget *parent)
    : DStackedWidget(parent)
{
    InitWidgets();
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
        MsgModel mm;
        mm.setMsgType(MSG_LEFTBAR_STATE);
        mm.setValue(QString::number(iIndex));

        dApp->m_pModelService->notifyMsg(E_TITLE_MSG_TYPE, mm.toJson());
    }
}

void DataStackedWidget::InitWidgets()
{
    ThumbnailWidget *tnWidget = new ThumbnailWidget(this);
    insertWidget(WIDGET_THUMBNAIL, tnWidget);

    insertWidget(WIDGET_catalog, new CatalogWidget(this));

    BookMarkWidget *bookMark = new BookMarkWidget(this);
    connect(bookMark, SIGNAL(sigSetBookMarkState(const int &, const int &)),
            tnWidget, SLOT(SlotSetBookMarkState(const int &, const int &)));

    insertWidget(WIDGET_BOOKMARK, bookMark);

    insertWidget(WIDGET_NOTE, new NotesWidget(this));
    insertWidget(WIDGET_SEARCH, new SearchResWidget(this));

    auto buffWidget = new BufferWidget(this);
    connect(this, SIGNAL(sigSearchWidgetState(const int &)), buffWidget, SLOT(SlotSetSpinnerState(const int &)));

    insertWidget(WIDGET_BUFFER, buffWidget);
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
    auto pWidget = this->findChild<DStackedWidget *>();
    if (pWidget) {
        int iIndex = pWidget->currentIndex();
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
}

void DataStackedWidget::onJumpToNextPage()
{
    auto pWidget = this->findChild<DStackedWidget *>();
    if (pWidget) {
        int iIndex = pWidget->currentIndex();
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
}

void DataStackedWidget::DeleteItemByKey()
{
    auto pWidget = this->findChild<DStackedWidget *>();
    if (pWidget) {
        int iIndex = pWidget->currentIndex();
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
}
