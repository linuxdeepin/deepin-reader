// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CatalogTreeView.h"
#include "Application.h"
#include "Utils.h"
#include "DocSheet.h"
#include "MsgHeader.h"
#include "ddlog.h"

#include <DFontSizeManager>
#include <DGuiApplicationHelper>

#include <QProxyStyle>
#include <QPainter>
#include <QStylePainter>
#include <QScroller>
#include <QHeaderView>
#include <QDebug>

class ActiveProxyStyle : public QProxyStyle
{
public:
    explicit ActiveProxyStyle(DWidget *parent)
    {
        // qCDebug(appLog) << "ActiveProxyStyle::ActiveProxyStyle() - Starting constructor";
        this->setParent(parent);
    }

    ~ActiveProxyStyle();

    void drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const
    {
        // qCDebug(appLog) << "ActiveProxyStyle::drawComplexControl() - Starting drawComplexControl";
        QStyleOptionComplex *op = const_cast<QStyleOptionComplex *>(option);
        op->state = option->state | QStyle::State_Active;
        QProxyStyle::drawComplexControl(control, op, painter, widget);
    }

    void drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const
    {
        // qCDebug(appLog) << "ActiveProxyStyle::drawControl() - Starting drawControl";
        QStyleOption *op = const_cast<QStyleOption *>(option);
        op->state = option->state | QStyle::State_Active;
        QProxyStyle::drawControl(element, op, painter, widget);
    }

    void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const
    {
        // qCDebug(appLog) << "ActiveProxyStyle::drawPrimitive() - Starting drawPrimitive";
        if (element == QStyle::PE_IndicatorBranch)
            painter->setPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().text().color());
        QStyleOption *op = const_cast<QStyleOption *>(option);
        op->state = option->state | QStyle::State_Active;
        QProxyStyle::drawPrimitive(element, op, painter, widget);
    }
};

ActiveProxyStyle::~ActiveProxyStyle()
{
    // qCDebug(appLog) << "ActiveProxyStyle::~ActiveProxyStyle() - Starting destructor";
    //NotTodo
}

class CatalogModel: public QStandardItemModel
{
public:
    explicit CatalogModel(QAbstractItemView *parent): QStandardItemModel(parent)
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
    // qCDebug(appLog) << "CatalogModel::data() - Starting data";
    if (role == Qt::SizeHintRole) {
        QSize size = QStandardItemModel::data(index, role).toSize();
        size.setHeight(m_parent->fontMetrics().height());
        return size;
    }
    // qCDebug(appLog) << "CatalogModel::data() - Returning data";
    return QStandardItemModel::data(index, role);
}

CatalogTreeView::CatalogTreeView(DocSheet *sheet, DWidget *parent)
    : DTreeView(parent), m_sheet(sheet)
{
    qCDebug(appLog) << "Creating CatalogTreeView for document:" << (sheet ? sheet->filePath() : "null");

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
    this->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    this->header()->setSectionResizeMode(1, QHeaderView::Fixed);
    this->viewport()->setAutoFillBackground(false);
    this->setContentsMargins(0, 0, 0, 0);

    connect(this, SIGNAL(collapsed(const QModelIndex &)), SLOT(slotCollapsed(const QModelIndex &)));
    connect(this, SIGNAL(expanded(const QModelIndex &)), SLOT(slotExpanded(const QModelIndex &)));
    connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(onItemClicked(QModelIndex)));

    connect(dApp, &DApplication::fontChanged, this, &CatalogTreeView::onFontChanged);

    QScroller::grabGesture(this, QScroller::TouchGesture);//滑动
    qCDebug(appLog) << "CatalogTreeView::CatalogTreeView() - Constructor completed";
}

CatalogTreeView::~CatalogTreeView()
{
    // qCDebug(appLog) << "Destroying CatalogTreeView";
}

void CatalogTreeView::setRightControl(bool hasControl)
{
    // qCDebug(appLog) << "CatalogTreeView::setRightControl() - Setting right control:" << hasControl;
    rightnotifypagechanged = hasControl;
}

void CatalogTreeView::parseCatalogData(const deepin_reader::Section &ol, QStandardItem *parentItem)
{
    qCDebug(appLog) << "Parsing catalog section:" << ol.title << "with" << ol.children.size() << "children";

    foreach (auto s, ol.children) { //  2级显示
        if (s.nIndex >= 0) {
            auto itemList = getItemList(s.title, s.nIndex, s.offsetPointF.x(), s.offsetPointF.y());
            parentItem->appendRow(itemList);

            foreach (auto s1, s.children) { //  3级显示
                auto itemList1 = getItemList(s1.title, s1.nIndex, s1.offsetPointF.x(), s1.offsetPointF.y());
                itemList.at(0)->appendRow(itemList1);
            }
        }
    }
    qCDebug(appLog) << "CatalogTreeView::parseCatalogData() - Completed";
}

QList<QStandardItem *> CatalogTreeView::getItemList(const QString &title, const int &index, const qreal  &realleft, const qreal &realtop)
{
    qCDebug(appLog) << "CatalogTreeView::getItemList() - Creating item for:" << title << "page:" << index;
    QStandardItem *item = new QStandardItem(title);
    item->setData(index);
    item->setData(realleft, Qt::UserRole + 2);
    item->setData(realtop, Qt::UserRole + 3);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QStandardItem *item1 = new QStandardItem(QString::number(index + 1));
    item1->setData(index);
    item1->setData(realleft, Qt::UserRole + 2);
    item1->setData(realtop, Qt::UserRole + 3);
    item1->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QColor color = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().textTips().color();
    item1->setForeground(QBrush(color));

    qCDebug(appLog) << "CatalogTreeView::getItemList() - Returning item list";
    return QList<QStandardItem *>() << item << item1;
}

void CatalogTreeView::handleOpenSuccess()
{
    qCDebug(appLog) << "Handling document open success, building catalog tree";

    auto model = reinterpret_cast<QStandardItemModel *>(this->model());
    if (model) {
        qCDebug(appLog) << "CatalogTreeView::handleOpenSuccess() - Clearing model";
        model->clear();

        if (nullptr == m_sheet) {
            qCritical() << "Cannot build catalog tree - document sheet is null";
            return;
        }

        m_index = m_sheet->currentIndex();
        const deepin_reader::Outline &ol = m_sheet->outline();
        for (const deepin_reader::Section &s : ol) {   //root
            if (s.nIndex >= 0) {
                auto itemList = getItemList(s.title, s.nIndex, s.offsetPointF.x(), s.offsetPointF.y());
                model->appendRow(itemList);
                parseCatalogData(s, itemList.at(0));
            }
        }
        setIndex(m_index);
    }
    resizeCoulumnWidth();
    qCDebug(appLog) << "CatalogTreeView::handleOpenSuccess() - Completed";
}

void CatalogTreeView::slotCollapsed(const QModelIndex &index)
{
    qCDebug(appLog) << "Catalog tree item collapsed:" << index.data().toString();

    Q_UNUSED(index);

    if (nullptr == m_sheet) {
        qCWarning(appLog) << "Cannot handle collapse event - document sheet is null";
        return;
    }

    setIndex(m_index, m_title);
    qCDebug(appLog) << "CatalogTreeView::slotCollapsed() - Completed";
}

void CatalogTreeView::slotExpanded(const QModelIndex &index)
{
    qCDebug(appLog) << "Catalog tree item expanded:" << index.data().toString();

    Q_UNUSED(index);

    if (nullptr == m_sheet) {
        qCWarning(appLog) << "Cannot handle expand event - document sheet is null";
        return;
    }

    setIndex(m_index, m_title);
    qCDebug(appLog) << "CatalogTreeView::slotExpanded() - Completed";
}

void CatalogTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if (!rightnotifypagechanged) {

        if (nullptr == m_sheet) {
            qCritical() << "Cannot navigate - document sheet is null";
            return;
        }

        int nIndex = current.data(Qt::UserRole + 1).toInt();
        double left = current.data(Qt::UserRole + 2).toDouble();
        double top = current.data(Qt::UserRole + 3).toDouble();
        m_title = current.data(Qt::DisplayRole).toString();
        if (nIndex >= 0)
            m_sheet->jumpToOutline(left, top, nIndex);
    }
    rightnotifypagechanged = false;

    qCDebug(appLog) << "CatalogTreeView::currentChanged() - Completed";
    return DTreeView::currentChanged(current, previous);
}

void CatalogTreeView::onItemClicked(const QModelIndex &current)
{
    qCDebug(appLog) << "Catalog item clicked:" << current.data().toString() << "page:" << current.data(Qt::UserRole + 1).toInt();

    if (nullptr == m_sheet) {
        qCritical() << "Cannot navigate to clicked item - document sheet is null";
        return;
    }

    int nIndex = current.data(Qt::UserRole + 1).toInt();
    double left = current.data(Qt::UserRole + 2).toDouble();
    double top = current.data(Qt::UserRole + 3).toDouble();
    m_title = current.data(Qt::DisplayRole).toString();

    if (nIndex >= 0)
        m_sheet->jumpToOutline(left, top, nIndex);

    qCDebug(appLog) << "CatalogTreeView::onItemClicked() - Completed";
}

void CatalogTreeView::resizeEvent(QResizeEvent *event)
{
    // qCDebug(appLog) << "CatalogTreeView::resizeEvent() - Starting resize event";
    DTreeView::resizeEvent(event);
    resizeCoulumnWidth();
    // qCDebug(appLog) << "CatalogTreeView::resizeEvent() - Completed";
}

void CatalogTreeView::mousePressEvent(QMouseEvent *event)
{
    // qCDebug(appLog) << "CatalogTreeView::mousePressEvent() - Starting mouse press event";
    rightnotifypagechanged = false;
    DTreeView::mousePressEvent(event);
    // qCDebug(appLog) << "CatalogTreeView::mousePressEvent() - Completed";
}

void CatalogTreeView::keyPressEvent(QKeyEvent *event)
{
    // qCDebug(appLog) << "CatalogTreeView::keyPressEvent() - Starting key press event";
    rightnotifypagechanged = false;
    DTreeView::keyPressEvent(event);
    // qCDebug(appLog) << "CatalogTreeView::keyPressEvent() - Completed";
}

void CatalogTreeView::setIndex(int index, const QString &title)
{
    qCDebug(appLog) << "Setting catalog selection - page:" << index << "title:" << (title.isEmpty() ? "[any]" : title);

    m_index = index;
    m_title = title;
    this->clearSelection();

    auto model = reinterpret_cast<QStandardItemModel *>(this->model());
    if (model) {
        qCDebug(appLog) << "CatalogTreeView::setIndex() - Finding items";
        const QList<QStandardItem *> &itemList = model->findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
        foreach (QStandardItem *item, itemList) {
            int itemIndex = item->data().toInt();
            if (itemIndex == index && (title.isEmpty() || title == item->data(Qt::DisplayRole).toString())) {
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
    qCDebug(appLog) << "CatalogTreeView::setIndex() - Completed";
}

void CatalogTreeView::resizeCoulumnWidth()
{
    qCDebug(appLog) << "CatalogTreeView::resizeCoulumnWidth() - Resizing column width";
    if (m_sheet) {
        qCDebug(appLog) << "CatalogTreeView::resizeCoulumnWidth() - Resizing column width";
        int maxPageColumnWid = this->fontMetrics().horizontalAdvance(QString::number(m_sheet->pageCount())) + 34;
        this->setColumnWidth(0, this->width() - maxPageColumnWid);
        this->setColumnWidth(1, maxPageColumnWid);
    }
    qCDebug(appLog) << "CatalogTreeView::resizeCoulumnWidth() - Completed";
}

void CatalogTreeView::nextPage()
{
    qCDebug(appLog) << "Navigating to next catalog item";

    const QModelIndex &newCurrent = this->moveCursor(QAbstractItemView::MoveDown, Qt::NoModifier);
    scrollToIndex(newCurrent);
}

void CatalogTreeView::pageDownPage()
{
    qCDebug(appLog) << "Page down navigation in catalog";

    const QModelIndex &newCurrent = this->moveCursor(QAbstractItemView::MovePageDown, Qt::NoModifier);
    scrollToIndex(newCurrent);
}

void CatalogTreeView::prevPage()
{
    qCDebug(appLog) << "Navigating to previous catalog item";

    const QModelIndex &newCurrent = this->moveCursor(QAbstractItemView::MoveUp, Qt::NoModifier);
    scrollToIndex(newCurrent);
}

void CatalogTreeView::pageUpPage()
{
    qCDebug(appLog) << "Page up navigation in catalog";

    const QModelIndex &newCurrent = this->moveCursor(QAbstractItemView::MovePageUp, Qt::NoModifier);
    scrollToIndex(newCurrent);
}

void CatalogTreeView::scrollToIndex(const QModelIndex &newIndex)
{
    qCDebug(appLog) << "Scrolling to catalog index:" << newIndex.data().toString();

    if (newIndex.isValid()) {
        qCDebug(appLog) << "CatalogTreeView::scrollToIndex() - Scrolling to index";
        rightnotifypagechanged = false;
        currentChanged(newIndex, currentIndex());
        this->selectionModel()->select(newIndex, QItemSelectionModel::SelectCurrent);
        this->setCurrentIndex(newIndex);
    }
}

void CatalogTreeView::onFontChanged(const QFont &font)
{
    // qCDebug(appLog) << "Handling font change in catalog view";

    Q_UNUSED(font);
    resizeCoulumnWidth();
}
