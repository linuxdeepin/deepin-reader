// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#include "CatalogOutlineModel.h"
#include <gtest/gtest.h>

using namespace deepin_reader;

TEST(CatalogOutlineModel, PreservesTargetlessParentsAndDeepChildren)
{
    Section leaf;
    leaf.title = QStringLiteral("leaf");
    leaf.nIndex = 2;
    Section root = leaf;
    for (int i = 0; i < 8; ++i) {
        Section parent;
        parent.title = QStringLiteral("group %1").arg(i);
        parent.children.append(root);
        root = parent;
    }
    QStandardItemModel model;
    appendCatalogSections(model.invisibleRootItem(), {root});
    ASSERT_EQ(model.rowCount(), 1);
    QStandardItem *item = model.item(0);
    EXPECT_EQ(model.item(0, 1)->text(), QString());
    EXPECT_EQ(item->data(CatalogPageRole).toInt(), -1);
    for (int i = 0; i < 8; ++i) {
        ASSERT_EQ(item->rowCount(), 1);
        item = item->child(0);
    }
    EXPECT_EQ(item->text(), QStringLiteral("leaf"));
    EXPECT_EQ(item->data(CatalogPageRole).toInt(), 2);
}

TEST(CatalogOutlineModel, KeepsLegacyPageAndOffsetRolesInBothColumns)
{
    Section section;
    section.title = QStringLiteral("chapter");
    section.nIndex = 3;
    section.offsetPointF = QPointF(12, 34);
    QStandardItemModel model;
    appendCatalogSections(model.invisibleRootItem(), {section});
    ASSERT_EQ(model.rowCount(), 1);
    ASSERT_EQ(model.columnCount(), 2);
    EXPECT_EQ(model.item(0, 1)->text(), QStringLiteral("4"));
    for (int c = 0; c < 2; ++c) {
        EXPECT_EQ(model.item(0, c)->data(CatalogPageRole).toInt(), 3);
        EXPECT_EQ(model.item(0, c)->data(CatalogLeftRole).toDouble(), 12);
        EXPECT_EQ(model.item(0, c)->data(CatalogTopRole).toDouble(), 34);
    }
}

TEST(CatalogOutlineModel, PreservesSiblingOrderAndHandlesEmptyOutline)
{
    QStandardItemModel model;
    appendCatalogSections(nullptr, {});
    appendCatalogSections(model.invisibleRootItem(), {});
    EXPECT_EQ(model.rowCount(), 0);
    Section a, b;
    a.title = QStringLiteral("A");
    b.title = QStringLiteral("B");
    appendCatalogSections(model.invisibleRootItem(), {a, b});
    ASSERT_EQ(model.rowCount(), 2);
    EXPECT_EQ(model.item(0)->text(), QStringLiteral("A"));
    EXPECT_EQ(model.item(1)->text(), QStringLiteral("B"));
}

TEST(CatalogOutlineModel, TypedTargetsAndExpansionArePreservedInBothColumns)
{
    Section section;
    section.title = QStringLiteral("typed");
    NavigationDestination destination;
    destination.pageIndex = 2;
    section.navigation = NavigationTarget{destination, {}};
    section.expanded = false;
    QStandardItemModel model;
    appendCatalogSections(model.invisibleRootItem(), {section});
    ASSERT_EQ(model.rowCount(), 1);
    EXPECT_EQ(model.item(0, 1)->text(), QStringLiteral("3"));
    for (int c = 0; c < 2; ++c) {
        const auto *item = model.item(0, c);
        EXPECT_EQ(item->data(CatalogPageRole).toInt(), 2);
        const auto target = item->data(CatalogNavigationRole).value<NavigationTarget>();
        ASSERT_TRUE(target.destination.has_value());
        EXPECT_EQ(target.destination->pageIndex, 2);
        EXPECT_TRUE(item->data(CatalogExpandedRole).isValid());
        EXPECT_FALSE(item->data(CatalogExpandedRole).toBool());
    }
}

TEST(CatalogOutlineModel, ExternalAndInvalidTargetsNeverInheritLegacyPage)
{
    Section external, invalid;
    external.nIndex = 9;
    external.navigation = NavigationTarget{{}, QUrl("https://example.invalid/")};
    invalid.nIndex = 8;
    invalid.navigation = NavigationTarget{};
    QStandardItemModel model;
    appendCatalogSections(model.invisibleRootItem(), {external, invalid});
    ASSERT_EQ(model.rowCount(), 2);
    for (int i = 0; i < 2; ++i) {
        EXPECT_EQ(model.item(i)->data(CatalogPageRole).toInt(), -1);
        EXPECT_TRUE(model.item(i, 1)->text().isEmpty());
        EXPECT_TRUE(model.item(i)->data(CatalogNavigationRole).isValid());
    }
}

TEST(CatalogOutlineModel, StoredExpansionIncludingEmptyListOverridesDefaultsWithoutSignals)
{
    Section leaf, child, root;
    leaf.title = QStringLiteral("leaf");
    child.title = QStringLiteral("child");
    child.expanded = true;
    child.children = {leaf};
    root.title = QStringLiteral("root");
    root.expanded = true;
    root.children = {child};
    QStandardItemModel model;
    appendCatalogSections(model.invisibleRootItem(), {root});
    QTreeView view;
    view.setModel(&model);
    int signalCount = 0;
    QObject::connect(&view, &QTreeView::expanded, [&signalCount] { ++signalCount; });
    QObject::connect(&view, &QTreeView::collapsed, [&signalCount] { ++signalCount; });
    const QModelIndex rootIndex = model.index(0, 0);
    const QModelIndex childIndex = model.index(0, 0, rootIndex);
    applyCatalogExpansion(&view);
    EXPECT_TRUE(view.isExpanded(rootIndex));
    EXPECT_TRUE(view.isExpanded(childIndex));
    applyCatalogExpansion(&view, QStringList{QStringLiteral("root")});
    EXPECT_TRUE(view.isExpanded(rootIndex));
    EXPECT_FALSE(view.isExpanded(childIndex));
    applyCatalogExpansion(&view, QStringList{});
    EXPECT_FALSE(view.isExpanded(rootIndex));
    EXPECT_FALSE(view.isExpanded(childIndex));
    EXPECT_EQ(signalCount, 0);
}
