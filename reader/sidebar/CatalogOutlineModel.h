// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef CATALOGOUTLINEMODEL_H
#define CATALOGOUTLINEMODEL_H

#include "Model.h"
#include <QStandardItem>
#include <QTreeView>

namespace deepin_reader {
enum CatalogRole {
    CatalogPageRole = Qt::UserRole + 1,
    CatalogLeftRole,
    CatalogTopRole,
    CatalogNavigationRole,
    CatalogExpandedRole
};
QList<QStandardItem *> catalogRow(const Section &section);
void appendCatalogSections(QStandardItem *parent, const Outline &outline);
void applyCatalogExpansion(QTreeView *view, const std::optional<QStringList> &saved = std::nullopt);
}
#endif
