// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#include "CatalogOutlineModel.h"
#include <QSet>
#include <QSignalBlocker>

namespace deepin_reader {
void applyCatalogExpansion(QTreeView *view, const std::optional<QStringList> &saved)
{
    if (!view || !view->model())
        return;
    const QSignalBlocker blocker(view);
    QSet<QString> paths;
    if (saved)
        paths = QSet<QString>(saved->cbegin(), saved->cend());
    struct Pending { QModelIndex parent; QString path; };
    QVector<Pending> pending{{QModelIndex(), QString()}};
    while (!pending.isEmpty()) {
        const Pending current = pending.takeLast();
        for (int row = 0; row < view->model()->rowCount(current.parent); ++row) {
            const QModelIndex index = view->model()->index(row, 0, current.parent);
            const QString title = index.data().toString();
            const QString path = current.parent.isValid() ? current.path + QLatin1Char('/') + title : title;
            const QVariant expanded = index.data(CatalogExpandedRole);
            if (saved || expanded.isValid())
                view->setExpanded(index, saved ? paths.contains(path) : expanded.toBool());
            pending.append({index, path});
        }
    }
}

QList<QStandardItem *> catalogRow(const Section &section)
{
    int pageIndex = section.nIndex;
    if (section.navigation) {
        pageIndex = section.navigation->isValid() && section.navigation->destination
            ? section.navigation->destination->pageIndex : -1;
    }
    auto *title = new QStandardItem(section.title);
    auto *page = new QStandardItem(pageIndex >= 0 ? QString::number(qint64(pageIndex) + 1) : QString());
    const QList<QStandardItem *> row{title, page};
    for (QStandardItem *item : row) {
        item->setData(pageIndex, CatalogPageRole);
        item->setData(section.offsetPointF.x(), CatalogLeftRole);
        item->setData(section.offsetPointF.y(), CatalogTopRole);
        if (section.navigation)
            item->setData(QVariant::fromValue(*section.navigation), CatalogNavigationRole);
        if (section.expanded)
            item->setData(*section.expanded, CatalogExpandedRole);
    }
    title->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    page->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    return row;
}

void appendCatalogSections(QStandardItem *parent, const Outline &outline)
{
    if (!parent)
        return;
    struct Pending { QStandardItem *parent; const Section *section; };
    QVector<Pending> pending;
    for (auto it = outline.crbegin(); it != outline.crend(); ++it)
        pending.append({parent, &*it});
    while (!pending.isEmpty()) {
        const Pending current = pending.takeLast();
        const auto row = catalogRow(*current.section);
        current.parent->appendRow(row);
        const auto &children = current.section->children;
        for (auto it = children.crbegin(); it != children.crend(); ++it)
            pending.append({row.first(), &*it});
    }
}
}
