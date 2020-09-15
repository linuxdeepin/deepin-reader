/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     wangzhixuan<wangzhixuan@uniontech.com>
 *
 * Maintainer: wangzhixuan<wangzhixuan@uniontech.com>
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
#include "CatalogTreeView.h"
#include "Application.h"
#include "Utils.h"
#include "DocSheet.h"
#include "MsgHeader.h"

#include <DFontSizeManager>
#include <DGuiApplicationHelper>

#include <QDebug>
#include <QProxyStyle>
#include <QPainter>
#include <QStylePainter>
#include <QScroller>
#include <QHeaderView>

class ActiveProxyStyle : public QProxyStyle
{
public:
    ActiveProxyStyle(DWidget *parent)
    {
        this->setParent(parent);
    }

    virtual void drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const
    {
        QStyleOptionComplex *op = const_cast<QStyleOptionComplex *>(option);
        op->state = option->state | QStyle::State_Active;
        QProxyStyle::drawComplexControl(control, op, painter, widget);
    }
    virtual void drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const
    {
        QStyleOption *op = const_cast<QStyleOption *>(option);
        op->state = option->state | QStyle::State_Active;
        QProxyStyle::drawControl(element, op, painter, widget);
    }

    virtual void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const
    {
        if (element == QStyle::PE_IndicatorBranch)
            painter->setPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().text().color());
        QStyleOption *op = const_cast<QStyleOption *>(option);
        op->state = option->state | QStyle::State_Active;
        QProxyStyle::drawPrimitive(element, op, painter, widget);
    }
};

class CatalogModel: public QStandardItemModel
{
public:
    CatalogModel(QAbstractItemView *parent): QStandardItemModel(parent)
    {
        m_parent = parent;
    }

protected:
    QVariant data(const QModelIndex &index, int role) const;

private:
    QAbstractItemView *m_parent;
};

QVariant CatalogModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::SizeHintRole) {
        QSize size = QStandardItemModel::data(index, role).toSize();
        size.setHeight(m_parent->fontMetrics().height());
        return size;
    }
    return QStandardItemModel::data(index, role);
}

CatalogTreeView::CatalogTreeView(DocSheet *sheet, DWidget *parent)
    : DTreeView(parent), m_sheet(sheet)
{
    ActiveProxyStyle *style = new ActiveProxyStyle(this);
    setStyle(style);
    setFrameShape(QFrame::NoFrame);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    CatalogModel *pModel = new CatalogModel(this);
    this->setModel(pModel);
    pModel->setColumnCount(2);

    this->header()->setHidden(true);
    this->header()->setDefaultSectionSize(18);
    this->header()->setMinimumSectionSize(18);
    this->header()->setStretchLastSection(false);
    this->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    this->header()->setSectionResizeMode(1, QHeaderView::Fixed);
    this->viewport()->setAutoFillBackground(false);
    this->setContentsMargins(0, 0, 0, 0);

    connect(this, SIGNAL(collapsed(const QModelIndex &)), SLOT(slotCollapsed(const QModelIndex &)));
    connect(this, SIGNAL(expanded(const QModelIndex &)), SLOT(slotExpanded(const QModelIndex &)));
    connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(onItemClicked(QModelIndex)));

    QScroller::grabGesture(this, QScroller::TouchGesture);//滑动
}

CatalogTreeView::~CatalogTreeView()
{

}

void CatalogTreeView::setRightControl(bool hasControl)
{
    rightnotifypagechanged = hasControl;
}

void CatalogTreeView::parseCatalogData(const deepin_reader::Section &ol, QStandardItem *parentItem)
{
    foreach (auto s, ol.children) { //  2级显示
        if (s.link.page > 0) {
            auto itemList = getItemList(s.title, s.link.page, s.link.left, s.link.top);
            parentItem->appendRow(itemList);

            foreach (auto s1, s.children) { //  3级显示
                if (s1.link.page > 0) {
                    auto itemList1 = getItemList(s1.title, s1.link.page, s1.link.left, s1.link.top);
                    itemList.at(0)->appendRow(itemList1);
                }
            }
        }
    }
}

QList<QStandardItem *> CatalogTreeView::getItemList(const QString &title, const int &index, const qreal  &realleft, const qreal &realtop)
{
    auto item = new QStandardItem(title);
    item->setData(index);
    item->setData(realleft, Qt::UserRole + 2);
    item->setData(realtop, Qt::UserRole + 3);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto item1 = new QStandardItem(QString::number(index));
    item1->setData(index);
    item1->setData(realleft, Qt::UserRole + 2);
    item1->setData(realtop, Qt::UserRole + 3);
    item1->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QColor color = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().textTips().color();
    item1->setForeground(QBrush(color));

    return QList<QStandardItem *>() << item << item1;
}

//  文档打开成功, 加载对应目录
void CatalogTreeView::handleOpenSuccess()
{
    auto model = reinterpret_cast<QStandardItemModel *>(this->model());
    if (model) {
        model->clear();

        if (nullptr == m_sheet)
            return;

        m_index = m_sheet->currentIndex();
        const deepin_reader::Outline &ol = m_sheet->outline();
        for (const deepin_reader::Section &s : ol) {   //root
            if (s.link.page > 0) {
                auto itemList = getItemList(s.title, s.link.page, s.link.left, s.link.top);
                model->appendRow(itemList);
                parseCatalogData(s, itemList.at(0));
            }
        }
        setIndex(m_index);
    }
    resizeCoulumnWidth();
}

//  折叠 节点处理
void CatalogTreeView::slotCollapsed(const QModelIndex &index)
{
    Q_UNUSED(index);

    if (nullptr == m_sheet)
        return;

    setIndex(m_index, m_title);
}

//  展开 节点处理
void CatalogTreeView::slotExpanded(const QModelIndex &index)
{
    Q_UNUSED(index);

    if (nullptr == m_sheet)
        return;

    setIndex(m_index, m_title);
}

//  实现 上下左键 跳转
void CatalogTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if (!rightnotifypagechanged) {

        if (nullptr == m_sheet)
            return;

        int nPage = current.data(Qt::UserRole + 1).toInt();
        double left = current.data(Qt::UserRole + 2).toDouble();
        double top = current.data(Qt::UserRole + 3).toDouble();
        m_title = current.data(Qt::DisplayRole).toString();
        if (nPage >= 1)
            m_sheet->jumpToOutline(left, top, static_cast<unsigned int>(nPage - 1));
    }
    rightnotifypagechanged = false;

    return DTreeView::currentChanged(current, previous);
}

void CatalogTreeView::onItemClicked(const QModelIndex &current)
{
    if (nullptr == m_sheet)
        return;

    int nPage = current.data(Qt::UserRole + 1).toInt();
    double left = current.data(Qt::UserRole + 2).toDouble();
    double top = current.data(Qt::UserRole + 3).toDouble();
    m_title = current.data(Qt::DisplayRole).toString();

    if (nPage >= 1)
        m_sheet->jumpToOutline(left, top, static_cast<unsigned int>(nPage - 1));
}

//  窗口大小变化, 列的宽度随之变化
void CatalogTreeView::resizeEvent(QResizeEvent *event)
{
    DTreeView::resizeEvent(event);
    resizeCoulumnWidth();
}

void CatalogTreeView::mousePressEvent(QMouseEvent *event)
{
    rightnotifypagechanged = false;
    DTreeView::mousePressEvent(event);
}

void CatalogTreeView::keyPressEvent(QKeyEvent *event)
{
    rightnotifypagechanged = false;
    DTreeView::keyPressEvent(event);
}

void CatalogTreeView::setIndex(int index, const QString &title)
{
    m_index = index;
    m_title = title;
    this->clearSelection();

    auto model = reinterpret_cast<QStandardItemModel *>(this->model());
    if (model) {
        const QList<QStandardItem *> &itemList = model->findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
        foreach (QStandardItem *item, itemList) {
            int itemPage = item->data().toInt();
            if (itemPage == index + 1 && (title.isEmpty() || title == item->data(Qt::DisplayRole).toString())) {
                QList<QStandardItem *> parentlst;
                parentlst << item;
                QStandardItem *parent = item->parent();
                while (parent) {
                    parentlst << parent;
                    parent = parent->parent();
                }

                for (int i = parentlst.size() - 1; i >= 0; i--) {
                    const QModelIndex &modelIndex = parentlst.at(i)->index();
                    if (!this->isExpanded(modelIndex) || item->index() == modelIndex) {
                        this->selectionModel()->select(modelIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
                        break;
                    }
                }
                break;
            }
        }
    }
}

void CatalogTreeView::resizeCoulumnWidth()
{
    if (m_sheet) {
        int maxPageColumnWid = this->fontMetrics().width(QString::number(m_sheet->pagesNumber())) + 32;
        this->setColumnWidth(0, this->width() - maxPageColumnWid);
        this->setColumnWidth(1, maxPageColumnWid);
    }
}

void CatalogTreeView::nextPage()
{
    const QModelIndex &newCurrent = this->moveCursor(QAbstractItemView::MoveDown, Qt::NoModifier);
    scrollToIndex(newCurrent);
}

void CatalogTreeView::pageDownPage()
{
    const QModelIndex &newCurrent = this->moveCursor(QAbstractItemView::MovePageDown, Qt::NoModifier);
    scrollToIndex(newCurrent);
}

void CatalogTreeView::prevPage()
{
    const QModelIndex &newCurrent = this->moveCursor(QAbstractItemView::MoveUp, Qt::NoModifier);
    scrollToIndex(newCurrent);
}

void CatalogTreeView::pageUpPage()
{
    const QModelIndex &newCurrent = this->moveCursor(QAbstractItemView::MovePageUp, Qt::NoModifier);
    scrollToIndex(newCurrent);
}

void CatalogTreeView::scrollToIndex(const QModelIndex &newIndex)
{
    if (newIndex.isValid()) {
        rightnotifypagechanged = false;
        currentChanged(newIndex, currentIndex());
        this->selectionModel()->select(newIndex, QItemSelectionModel::SelectCurrent);
        this->setCurrentIndex(newIndex);
    }
}
