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
#include "PageSearchThread.h"
#include "BrowserPage.h"
#include "SheetRenderer.h"

#include <QRectF>
#include <QDebug>

QMap<QChar, QChar> PageSearchThread::m_cjktokangximap;

PageSearchThread::PageSearchThread(QObject *parent) : QThread(parent)
{

}

PageSearchThread::~PageSearchThread()
{
    m_quit = true;
    this->wait();
}

void PageSearchThread::startSearch(DocSheet *sheet, QString text)
{
    stopSearch();
    m_quit = false;
    m_startIndex = 0;
    m_sheet = sheet;
    m_searchText = text;
    start();
}

void PageSearchThread::stopSearch()
{
    m_quit = true;
    this->wait();
    m_sheet = nullptr;
}

void PageSearchThread::run()
{
    if (nullptr == m_sheet)
        return;

    initCJKtoKangxi();

    int size = m_sheet->pageCount();
    QString searchTextKangxi = m_searchText;
    for (int i = 0; i < m_searchText.size(); i++) {
        if (m_cjktokangximap.contains(m_searchText.at(i))) {
            searchTextKangxi.replace(i, 1, m_cjktokangximap.value(m_searchText.at(i)));
        }
    }

    for (int index = 0; index < size; index++) {
        if (m_quit) return;

        SearchResult searchres;

        searchres.page = index + 1;

        searchres.rects = m_sheet->renderer()->search(index, m_searchText, false, false);
        if (searchTextKangxi != m_searchText) { // 存在康熙字典部首字体
            searchres.rects.append(m_sheet->renderer()->search(index, searchTextKangxi, false, false));
        }

        //高亮被搜索内容
//        if (textrectLst.size() > 0)
//            page->setSearchHighlightRectf(textrectLst);

        //把搜索的范围中周边的文字取出用于左侧展示
        for (const QRectF &rec : searchres.rects) {
            if (m_quit)
                return;

            //获取搜索结果附近文字
            QRectF rctext = rec;
            rctext.setX(rctext.x() - 40);
            rctext.setWidth(rctext.width() + 80);

            const QString &text = m_sheet->renderer()->getText(index, rctext);

            if (!text.isEmpty()) {
                searchres.words << Word(text, rec);
            }
        }

        if (searchres.words.size() > 0)
            emit sigSearchReady(searchres);
    }
}

void PageSearchThread::initCJKtoKangxi()
{
    if (!m_cjktokangximap.isEmpty()) {
        return;
    }

    QFile file(":/CJK2Kangxi.dict");
    if (!file.open(QIODevice::ReadOnly)) {
        qInfo() << "open dictFile error :" << file.error();
        return;
    }

    QByteArray content = file.readAll();
    file.close();
    QTextStream stream(&content, QIODevice::ReadOnly);
    while (!stream.atEnd()) {
        const QStringList items = stream.readLine().split(QChar(':'));
        if (items.size() == 2) {
            m_cjktokangximap.insert(QChar(items[0].toInt(nullptr, 16)), QChar(items[1].toInt(nullptr, 16)));
        }
    }
}
