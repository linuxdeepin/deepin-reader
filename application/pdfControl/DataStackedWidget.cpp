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
#include "DocSheet.h"

DataStackedWidget::DataStackedWidget(DocSheet *sheet, DWidget *parent)
    : DStackedWidget(parent), m_sheet(sheet)
{
    InitWidgets();
}

void DataStackedWidget::handleRotate(int rotate)
{
    m_pThWidget->handleRotate(rotate);
    m_pBookMarkWidget->handleRotate(rotate);
    m_pNotesWidget->handleRotate(rotate);
    m_pSearchResWidget->handleRotate(rotate);
}

void DataStackedWidget::handlePage(int page)
{
    m_pThWidget->handlePage(page);
    m_pBookMarkWidget->handlePage(page);
    m_pCatalogWidget->handlePage(page);
}

void DataStackedWidget::handleBookMark(int page, int state)
{
    m_pBookMarkWidget->setBookMark(page, state);
}

void DataStackedWidget::handleFindOperation(const int &iType)
{
    if (iType == E_FIND_CONTENT) {
        m_pSearchResWidget->clearFindResult();
        setCurrentWidget(m_pSearchResWidget);
    } else if (iType == E_FIND_EXIT) {
        int nId = m_sheet->getOper(LeftIndex).toInt();
        if (nId == -1) {
            nId = 0;
        }
        setCurrentIndex(nId);
        m_pSearchResWidget->clearFindResult();
    }
}

void DataStackedWidget::handleFindContentComming(const stSearchRes &res)
{
    m_pSearchResWidget->handFindContentComming(res);
}

int DataStackedWidget::handleFindFinished()
{
    return m_pSearchResWidget->handleFindFinished();
}

int DataStackedWidget::qDealWithShortKey(const QString &shortCut)
{
    return (m_pBookMarkWidget) ? m_pBookMarkWidget->qDealWithShortKey(shortCut) : MSG_NO_OK;
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

    double scale = 1.0;
    double t_epsinon = 1.0;

    t_epsinon = m_scale - m_dScale;

    if ((t_epsinon < -EPSINON) || (t_epsinon > EPSINON)) {
        //刷新当前列表视图大小,如果缩放比例有变化的话
        m_dScale = m_scale;
        adaptWindowSize(scale);
    }

    //  前一个是 出来搜索结果了, 后一个是正在搜索, 两个都不需要保存在记录中
    if (currentWidget() != m_pSearchResWidget) {
        m_sheet->setData(LeftIndex, QString::number(iIndex));
    }
}

/**
 * @brief DataStackedWidget::adaptWindowSize
 * 缩略图列表自适应视窗大小
 * @param scale  缩放因子 大于0的数
 */
void DataStackedWidget::adaptWindowSize(const double &scale)
{
    if (scale < 0) {
        return;
    }

    m_scale = scale;

    QWidget *widget = this->currentWidget();

    if (widget == m_pThWidget) {
        if (m_pThWidget) {
            m_pThWidget->adaptWindowSize(scale);
        }
    }  else if (widget == m_pBookMarkWidget) {
        if (m_pBookMarkWidget) {
            m_pBookMarkWidget->adaptWindowSize(scale);
        }
    } else if (widget == m_pNotesWidget) {
        if (m_pNotesWidget) {
            m_pNotesWidget->adaptWindowSize(scale);
        }
    } else if (widget == m_pSearchResWidget) {
        if (m_pSearchResWidget) {
            m_pSearchResWidget->adaptWindowSize(scale);
        }
    }
}

void DataStackedWidget::handleUpdateThumbnail(const int &page)
{
    if (m_pThWidget) {
        m_pThWidget->updateThumbnail(page);
    }
    if (m_pBookMarkWidget) {
        m_pBookMarkWidget->updateThumbnail(page);
    }
    if (m_pNotesWidget) {
        m_pNotesWidget->updateThumbnail(page);
    }
    if (m_pSearchResWidget) {
        m_pSearchResWidget->updateThumbnail(page);
    }
}

void DataStackedWidget::handleAnntationMsg(const int &type, const QString &text)
{
    m_pNotesWidget->handleAnntationMsg(type, text);
}

void DataStackedWidget::InitWidgets()
{
    m_pThWidget = new ThumbnailWidget(m_sheet, this);
    addWidget(m_pThWidget);

    m_pCatalogWidget = new CatalogWidget(m_sheet, this);
    addWidget(m_pCatalogWidget);

    m_pBookMarkWidget = new BookMarkWidget(m_sheet, this);
    connect(m_pBookMarkWidget, SIGNAL(sigSetBookMarkState(const int &, const int &)), m_pThWidget, SLOT(SlotSetBookMarkState(const int &, const int &)));
    addWidget(m_pBookMarkWidget);

    m_pNotesWidget = new NotesWidget(m_sheet, this);
    connect(m_pNotesWidget, SIGNAL(sigDeleteContent(const int &, const QString &)), this, SIGNAL(sigDeleteAnntation(const int &, const QString &)));
    addWidget(m_pNotesWidget);

    m_pSearchResWidget = new SearchResWidget(m_sheet, this);
    addWidget(m_pSearchResWidget);

    setCurrentWidget(m_pThWidget);
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
    QWidget *widget = this->currentWidget();
    if (widget == m_pThWidget) {
        if (m_pThWidget) {
            m_pThWidget->prevPage();
        }
    }  else if (widget == m_pBookMarkWidget) {
        if (m_pBookMarkWidget) {
            m_pBookMarkWidget->prevPage();
        }
    } else if (widget == m_pNotesWidget) {
        if (m_pNotesWidget) {
            m_pNotesWidget->prevPage();
        }
    }
}

void DataStackedWidget::onJumpToNextPage()
{
    QWidget *widget = this->currentWidget();
    if (widget == m_pThWidget) {
        if (m_pThWidget) {
            m_pThWidget->nextPage();
        }
    }  else if (widget == m_pBookMarkWidget) {
        if (m_pBookMarkWidget) {
            m_pBookMarkWidget->nextPage();
        }
    } else if (widget == m_pNotesWidget) {
        if (m_pNotesWidget) {
            m_pNotesWidget->nextPage();
        }
    }
}

void DataStackedWidget::DeleteItemByKey()
{
    QWidget *widget = this->currentWidget();
    if (widget == m_pBookMarkWidget) {
        if (m_pBookMarkWidget) {
            m_pBookMarkWidget->DeleteItemByKey();
        }
    } else if (widget == m_pNotesWidget) {
        if (m_pNotesWidget) {
            m_pNotesWidget->DeleteItemByKey();
        }
    }
}

void DataStackedWidget::handleOpenSuccess()
{
    int nId = m_sheet->getOper(LeftIndex).toInt();
    if (nId == -1) {
        nId = 0;
    }
    setCurrentIndex(nId);

    m_pThWidget->handleOpenSuccess();
    m_pCatalogWidget->handleOpenSuccess();
    m_pBookMarkWidget ->handleOpenSuccess();
    m_pNotesWidget->handleOpenSuccess();
}

void DataStackedWidget::showEvent(QShowEvent *event)
{
    DStackedWidget::showEvent(event);
    QTimer::singleShot(1000, this, [ = ] {
        dApp->setOpenFileOk(true);
    });
}

