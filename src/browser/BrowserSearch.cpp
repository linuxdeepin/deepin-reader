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
#include "BrowserSearch.h"
#include "BrowserPage.h"

#include <QDebug>

BrowserSearch::BrowserSearch(QObject *parent) : QThread(parent)
{

}

BrowserSearch::~BrowserSearch()
{
    m_quit = true;
    this->wait();
}

void BrowserSearch::startSearch(const QList<BrowserPage *> pagelst, const QString &searchText, int startIndex)
{
    stopSearch();
    m_quit = false;
    m_startIndex = startIndex;
    m_searchText = searchText;
    m_pagelst = pagelst;
    start();
}

void BrowserSearch::stopSearch()
{
    m_quit = true;
    m_pagelst.clear();
    this->wait();
}

void BrowserSearch::run()
{
    SearchResult searchres;
    int size = m_pagelst.size();
    for (int index = 0; index < size; index++) {
        if (m_quit) return;
        int curIndex = (index + m_startIndex) % size;
        BrowserPage *page = m_pagelst.at(curIndex);
        const QList< QRectF > &textrectLst = page->m_page->search(m_searchText, false, false);
        page->setSearchHighlightRectf(textrectLst);
        for (const QRectF &rec : textrectLst) {
            if (m_quit) return;
            //获取搜索结果附近文字
            QRectF rctext = rec;
            rctext.setX(rctext.x() - 40);
            rctext.setWidth(rctext.width() + 80);

            const QString &text = page->m_page->text(rctext);
            if (!text.isEmpty()) {
                searchres.page = page->itemIndex() + 1;
                searchres.words << Word(text, rec);
                emit sigSearchReady(searchres);
            }

        }
    }
}
