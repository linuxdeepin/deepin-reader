/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     leiyu
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

#include "SheetSidebar.h"
#include "DocSheet.h"
#include "thumbnail/ThumbnailWidget.h"
#include "catalog/CatalogWidget.h"
#include "bookmark/BookMarkWidget.h"
#include "note/NotesWidget.h"
#include "search/SearchResWidget.h"
#include "document/Model.h"
#include "threadmanager/ReaderImageThreadPoolManager.h"

#include <QButtonGroup>
#include <QVBoxLayout>
#include <QTimer>

SheetSidebar::SheetSidebar(DocSheet *parent, PreviewWidgesFlags widgesFlag)
    : CustomWidget(parent)
    , m_sheet(parent)
    , m_widgetsFlag(widgesFlag | PREVIEW_SEARCH)
{
    initWidget();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &SheetSidebar::onUpdateWidgetTheme);
}

SheetSidebar::~SheetSidebar()
{

}

void SheetSidebar::initWidget()
{
    m_scale           = 1.0;
    m_bOldVisible     = false;
    m_bOpenDocOpenSuccess = false;
    m_thumbnailWidget = nullptr;
    m_catalogWidget   = nullptr;
    m_bookmarkWidget  = nullptr;
    m_notesWidget     = nullptr;
    m_searchWidget    = nullptr;

    int tW = LEFTMINWIDTH;
    setMinimumWidth(tW);

    tW = LEFTMAXWIDTH;
    setMaximumWidth(tW);

    tW = LEFTNORMALWIDTH;
    resize(tW, this->height());

    QVBoxLayout *pVBoxLayout = new QVBoxLayout;
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->setSpacing(0);
    this->setLayout(pVBoxLayout);

    m_stackLayout = new QStackedLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(15, 0, 15, 0);

    m_btnGroup = new QButtonGroup(this);
    connect(m_btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(onBtnClicked(int)));
    if (m_widgetsFlag.testFlag(PREVIEW_THUMBNAIL)) {
        m_thumbnailWidget = new ThumbnailWidget(m_sheet, this);
        int index = m_stackLayout->addWidget(m_thumbnailWidget);
        DToolButton *btn = createBtn(tr("Thumbnails"), "thumbnail");
        m_btnGroup->addButton(btn, index);
        hLayout->addWidget(btn);
        hLayout->addStretch();
    }

    if (m_widgetsFlag.testFlag(PREVIEW_CATALOG)) {
        m_catalogWidget = new CatalogWidget(m_sheet, this);
        int index = m_stackLayout->addWidget(m_catalogWidget);
        DToolButton *btn = createBtn(tr("Catalog"), "catalog");
        m_btnGroup->addButton(btn, index);
        hLayout->addWidget(btn);
        hLayout->addStretch();
    }

    if (m_widgetsFlag.testFlag(PREVIEW_BOOKMARK)) {
        m_bookmarkWidget = new BookMarkWidget(m_sheet, this);
        int index = m_stackLayout->addWidget(m_bookmarkWidget);
        DToolButton *btn = createBtn(tr("Bookmarks"), "bookmark");
        m_btnGroup->addButton(btn, index);
        hLayout->addWidget(btn);
        hLayout->addStretch();
    }

    if (m_widgetsFlag.testFlag(PREVIEW_NOTE)) {
        m_notesWidget = new NotesWidget(m_sheet, this);
        int index = m_stackLayout->addWidget(m_notesWidget);
        DToolButton *btn = createBtn(tr("Annotations"), "annotation");
        m_btnGroup->addButton(btn, index);
        hLayout->addWidget(btn);
        hLayout->addStretch();
    }

    //remove last spaceitem
    QLayoutItem *item = hLayout->takeAt(hLayout->count() - 1);
    if (item) delete item;

    if (m_widgetsFlag.testFlag(PREVIEW_SEARCH)) {
        m_searchWidget = new SearchResWidget(m_sheet, this);
        m_searchId = m_stackLayout->addWidget(m_searchWidget);
        DToolButton *btn = createBtn("Search", "search");
        btn->setVisible(false);
        m_btnGroup->addButton(btn, m_searchId);
    }

    pVBoxLayout->addLayout(m_stackLayout);
    pVBoxLayout->addLayout(hLayout);
    onUpdateWidgetTheme();
    this->setVisible(false);
    m_btnGroup->buttonClicked(0);
}

void SheetSidebar::onBtnClicked(int index)
{
    m_btnGroup->button(index)->setChecked(true);
    m_stackLayout->setCurrentIndex(index);
    adaptWindowSize(m_scale);
    if (m_stackLayout->currentWidget() != m_searchWidget) {
        m_sheet->m_operation.sidebarIndex = index;
    }
    handWidgetDocOpenSuccess();
}

void SheetSidebar::setBookMark(int index, int state)
{
    if (m_bookmarkWidget) m_bookmarkWidget->handleBookMark(index, state);
    if (m_thumbnailWidget) m_thumbnailWidget->setBookMarkState(index, state);
}

void SheetSidebar::setCurrentPage(int page)
{
    if (m_thumbnailWidget) m_thumbnailWidget->handlePage(page - 1);
    if (m_bookmarkWidget) m_bookmarkWidget->handlePage(page - 1);
    if (m_catalogWidget) m_catalogWidget->handlePage(page - 1);
}

void SheetSidebar::handleOpenSuccess()
{
    m_bOpenDocOpenSuccess = true;
    this->setVisible(m_sheet->operation().sidebarVisible);
    int nId = qBound(0, m_sheet->operation().sidebarIndex, m_stackLayout->count() - 2);
    QAbstractButton *btn = m_btnGroup->button(nId);
    if (btn) m_btnGroup->buttonClicked(nId);
    handWidgetDocOpenSuccess();
}

void SheetSidebar::handWidgetDocOpenSuccess()
{
    if (m_bOpenDocOpenSuccess) {
        QTimer::singleShot(10, [this]() {
            QWidget *curWidget = m_stackLayout->currentWidget();
            if (curWidget == m_thumbnailWidget) {
                m_thumbnailWidget->handleOpenSuccess();
            } else if (curWidget == m_catalogWidget) {
                m_catalogWidget->handleOpenSuccess();
            } else if (curWidget == m_bookmarkWidget) {
                m_bookmarkWidget->handleOpenSuccess();
            } else if (curWidget == m_notesWidget) {
                m_notesWidget->handleOpenSuccess();
            }
        });
    }
}

void SheetSidebar::handleFindOperation(int type)
{
    if (type == E_FIND_CONTENT) {
        const QList<QAbstractButton *> &btns = m_btnGroup->buttons();
        for (QAbstractButton *btn : btns) btn->setEnabled(false);

        m_searchWidget->clearFindResult();
        QAbstractButton *btn = m_btnGroup->button(m_searchId);
        if (btn) m_btnGroup->buttonClicked(m_searchId);
        this->setVisible(true);
    } else if (type == E_FIND_EXIT) {
        const QList<QAbstractButton *> &btns = m_btnGroup->buttons();
        for (QAbstractButton *btn : btns) btn->setEnabled(true);

        int nId = qBound(0, m_sheet->operation().sidebarIndex, m_stackLayout->count() - 1);
        QAbstractButton *btn = m_btnGroup->button(nId);
        if (btn) m_btnGroup->buttonClicked(nId);
        this->setVisible(m_sheet->operation().sidebarVisible);
        m_searchWidget->clearFindResult();
    }
}

void SheetSidebar::handleFindContentComming(const deepin_reader::SearchResult &res)
{
    m_searchWidget->handFindContentComming(res);
}

int SheetSidebar::handleFindFinished()
{
    return m_searchWidget->handleFindFinished();
}

void SheetSidebar::handleRotate(int rotate)
{
    if (m_thumbnailWidget) m_thumbnailWidget->handleRotate(rotate);
}

void SheetSidebar::handleUpdateThumbnail(const int &index)
{
    CustomWidget *curWidget = dynamic_cast<CustomWidget *>(m_stackLayout->currentWidget());
    if (curWidget) curWidget->updateThumbnail(index);
}

void SheetSidebar::handleUpdatePartThumbnail(const int &index)
{
    const QPixmap &image = ReaderImageThreadPoolManager::getInstance()->getImageForDocSheet(m_sheet, index);
    if (image.isNull()) {
        handleUpdateThumbnail(index);
    }
}

void SheetSidebar::addAnnotation(deepin_reader::Annotation *anno)
{
    if (m_notesWidget)
        m_notesWidget->addNoteItem(anno);

    handleUpdateThumbnail(anno->page - 1);
}

void SheetSidebar::delteAnnotation(int index, const QString &uuid)
{
    if (m_notesWidget)
        m_notesWidget->deleteNoteItem(uuid);

    handleUpdateThumbnail(index);
}

DToolButton *SheetSidebar::createBtn(const QString &btnName, const QString &objName)
{
    int tW = 36;
    auto btn = new DToolButton(this);
    btn->setToolTip(btnName);
    btn->setObjectName(objName);
    btn->setFixedSize(QSize(tW, tW));
    btn->setIconSize(QSize(tW, tW));
    btn->setCheckable(true);
    return btn;
}

void SheetSidebar::resizeEvent(QResizeEvent *event)
{
    qreal scale = event->size().width() * 1.0 / LEFTMINWIDTH;
    adaptWindowSize(scale);
    CustomWidget::resizeEvent(event);
}

void SheetSidebar::adaptWindowSize(const double &scale)
{
    m_scale = scale;
    CustomWidget *curWidget = dynamic_cast<CustomWidget *>(m_stackLayout->currentWidget());
    if (curWidget) curWidget->adaptWindowSize(scale);
}

void SheetSidebar::keyPressEvent(QKeyEvent *event)
{
    QStringList pFilterList = QStringList() << Dr::key_pgUp << Dr::key_pgDown
                              << Dr::key_down << Dr::key_up
                              << Dr::key_left << Dr::key_right
                              << Dr::key_delete;
    QString key = Utils::getKeyshortcut(event);
    if (pFilterList.contains(key)) {
        dealWithPressKey(key);
    }
    CustomWidget::keyPressEvent(event);
}

void SheetSidebar::dealWithPressKey(const QString &sKey)
{
    if (sKey == Dr::key_up || sKey == Dr::key_pgUp || sKey == Dr::key_left) {
        onJumpToPrevPage();
    } else if (sKey == Dr::key_down || sKey == Dr::key_pgDown || sKey == Dr::key_right) {
        onJumpToNextPage();
    } else if (sKey == Dr::key_delete) {
        deleteItemByKey();
    }
}

void SheetSidebar::onJumpToPrevPage()
{
    QWidget *curWidget = m_stackLayout->currentWidget();
    if (curWidget == m_thumbnailWidget) {
        m_thumbnailWidget->prevPage();
    }  else if (curWidget == m_bookmarkWidget) {
        m_bookmarkWidget->prevPage();
    } else if (curWidget == m_notesWidget) {
        m_notesWidget->prevPage();
    }
}

void SheetSidebar::onJumpToNextPage()
{
    QWidget *curWidget = m_stackLayout->currentWidget();
    if (curWidget == m_thumbnailWidget) {
        m_thumbnailWidget->nextPage();
    }  else if (curWidget == m_bookmarkWidget) {
        m_bookmarkWidget->nextPage();
    } else if (curWidget == m_notesWidget) {
        m_notesWidget->nextPage();
    }
}

void SheetSidebar::deleteItemByKey()
{
    QWidget *widget = m_stackLayout->currentWidget();
    if (widget == m_bookmarkWidget) {
        m_bookmarkWidget->DeleteItemByKey();
    } else if (widget == m_notesWidget) {
        m_notesWidget->DeleteItemByKey();
    }
}

void SheetSidebar::onUpdateWidgetTheme()
{
    updateWidgetTheme();
    const QList<QAbstractButton *> &btns = m_btnGroup->buttons();
    for (QAbstractButton *btn : btns) {
        const QString &objName = btn->objectName();
        const QIcon &icon = QIcon::fromTheme(QString("dr_") + objName);//PF::getIcon(QString("dr_") + objName);
        btn->setIcon(icon);
    }
}
