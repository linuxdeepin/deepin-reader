// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SheetSidebar.h"
#include "DocSheet.h"
#include "ThumbnailWidget.h"
#include "CatalogWidget.h"
#include "BookMarkWidget.h"
#include "NotesWidget.h"
#include "SearchResWidget.h"
#include "Model.h"
#include "MsgHeader.h"
#include "Utils.h"
#include "ddlog.h"

#include <DPushButton>
#include <DGuiApplicationHelper>
#include <QDebug>

#include <QButtonGroup>
#include <QVBoxLayout>
#include <QTimer>

const int LEFTMINWIDTH = 266;
const int LEFTMAXWIDTH = 380;
SheetSidebar::SheetSidebar(DocSheet *parent, PreviewWidgesFlags widgesFlag)
    : BaseWidget(parent)
    , m_sheet(parent)
    , m_widgetsFlag(widgesFlag | PREVIEW_SEARCH)
{
    qCDebug(appLog) << "SheetSidebar created with flags:" << widgesFlag;
    initWidget();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &SheetSidebar::onUpdateWidgetTheme);
}

SheetSidebar::~SheetSidebar()
{
    qCDebug(appLog) << "SheetSidebar destroyed";
}

void SheetSidebar::initWidget()
{
    qCDebug(appLog) << "Initializing SheetSidebar UI";
    m_scale           = 1.0;
    m_bOldVisible     = false;
    m_bOpenDocOpenSuccess = false;
    m_thumbnailWidget = nullptr;
    m_catalogWidget   = nullptr;
    m_bookmarkWidget  = nullptr;
    m_notesWidget     = nullptr;
    m_searchWidget    = nullptr;

    setMinimumWidth(LEFTMINWIDTH);
    setMaximumWidth(LEFTMAXWIDTH);

    resize(LEFTMINWIDTH, this->height());

    QVBoxLayout *pVBoxLayout = new QVBoxLayout;
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->setSpacing(0);
    this->setLayout(pVBoxLayout);

    m_stackLayout = new QStackedLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(15, 5, 15, 5);

    QList<QWidget *> tabWidgetlst;

    if (m_widgetsFlag.testFlag(PREVIEW_THUMBNAIL)) {
        qCDebug(appLog) << "Creating thumbnail widget";
        m_thumbnailWidget = new ThumbnailWidget(m_sheet, this);
        int index = m_stackLayout->addWidget(m_thumbnailWidget);
        DToolButton *btn = createBtn(tr("Thumbnails"), "thumbnail");
        m_btnGroupMap.insert(index, btn);
        hLayout->addWidget(btn);
        hLayout->addStretch();

        connect(btn, &QToolButton::clicked, [this, index]() {
            onBtnClicked(index);
        });

        m_thumbnailWidget->setTabOrderWidget(tabWidgetlst);
    }

    if (m_widgetsFlag.testFlag(PREVIEW_CATALOG)) {
        qCDebug(appLog) << "Creating catalog widget";
        m_catalogWidget = new CatalogWidget(m_sheet, this);
        int index = m_stackLayout->addWidget(m_catalogWidget);
        DToolButton *btn = createBtn(tr("Catalog"), "catalog");
        m_btnGroupMap.insert(index, btn);
        hLayout->addWidget(btn);
        hLayout->addStretch();

        connect(btn, &QToolButton::clicked, [this, index]() {
            onBtnClicked(index);
        });
    }

    if (m_widgetsFlag.testFlag(PREVIEW_BOOKMARK)) {
        qCDebug(appLog) << "Creating bookmark widget";
        m_bookmarkWidget = new BookMarkWidget(m_sheet, this);
        int index = m_stackLayout->addWidget(m_bookmarkWidget);
        DToolButton *btn = createBtn(tr("Bookmarks"), "bookmark");
        m_btnGroupMap.insert(index, btn);
        hLayout->addWidget(btn);
        hLayout->addStretch();

        connect(btn, &QToolButton::clicked, [this, index]() {
            onBtnClicked(index);
        });

        m_bookmarkWidget->setTabOrderWidget(tabWidgetlst);
    }

    if (m_widgetsFlag.testFlag(PREVIEW_NOTE)) {
        qCDebug(appLog) << "Creating notes widget";
        m_notesWidget = new NotesWidget(m_sheet, this);
        int index = m_stackLayout->addWidget(m_notesWidget);
        DToolButton *btn = createBtn(tr("Annotations"), "annotation");
        m_btnGroupMap.insert(index, btn);
        hLayout->addWidget(btn);
        hLayout->addStretch();

        connect(btn, &QToolButton::clicked, [this, index]() {
            onBtnClicked(index);
        });

        m_notesWidget->setTabOrderWidget(tabWidgetlst);
    }

    //remove last spaceitem
    QLayoutItem *item = hLayout->takeAt(hLayout->count() - 1);
    if (item) delete item;

    if (m_widgetsFlag.testFlag(PREVIEW_SEARCH)) {
        qCDebug(appLog) << "Creating search widget";
        m_searchWidget = new SearchResWidget(m_sheet, this);
        m_searchId = m_stackLayout->addWidget(m_searchWidget);
        DToolButton *btn = createBtn("Search", "search");
        btn->setVisible(false);
        m_btnGroupMap.insert(m_searchId, btn);
        tabWidgetlst << btn;
    }

    pVBoxLayout->addLayout(m_stackLayout);
    pVBoxLayout->addLayout(hLayout);
    onUpdateWidgetTheme();
    this->setVisible(false);

    int nId = qBound(0, m_sheet->operation().sidebarIndex, m_stackLayout->count() - 1);
    onBtnClicked(nId);

    Utils::setObjectNoFocusPolicy(this);
    for (int i = 0; i < tabWidgetlst.size() - 1; i++) {
        QWidget::setTabOrder(tabWidgetlst.at(i), tabWidgetlst.at(i + 1));
    }
#ifdef DTKWIDGET_CLASS_DSizeMode
    if (DGuiApplicationHelper::instance()->sizeMode() == DGuiApplicationHelper::NormalMode) {
        setMinimumWidth(LEFTMINWIDTH);
        setMaximumWidth(LEFTMAXWIDTH);
        for(DToolButton *btn: m_btnGroupMap.values()) {
            if(btn) {
                btn->setFixedSize(QSize(36, 36));
            }
        }
    } else {
        setMinimumWidth(LEFTMINWIDTH-30);
        setMaximumWidth(LEFTMAXWIDTH);
        for(DToolButton *btn: m_btnGroupMap.values()) {
            if(btn) {
                btn->setFixedSize(QSize(24, 24));
            }
        }
    }
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [=](DGuiApplicationHelper::SizeMode sizeMode) {
        if (sizeMode == DGuiApplicationHelper::NormalMode) {
            setMinimumWidth(LEFTMINWIDTH);
            setMaximumWidth(LEFTMAXWIDTH);
            for(DToolButton *btn: m_btnGroupMap.values()) {
                if(btn) {
                    btn->setFixedSize(QSize(36, 36));
                }
            }
        } else {
            setMinimumWidth(LEFTMINWIDTH-30);
            setMaximumWidth(LEFTMAXWIDTH);
            for(DToolButton *btn: m_btnGroupMap.values()) {
                if(btn) {
                    btn->setFixedSize(QSize(24, 24));
                }
            }
        }
    });
#endif
}

void SheetSidebar::onBtnClicked(int index)
{
    qCDebug(appLog) << "Button clicked at index:" << index;
    if (!m_btnGroupMap.contains(index)) {
        qCWarning(appLog) << "Invalid button index:" << index;
        return;
    }

    for (auto iter = m_btnGroupMap.begin(); iter != m_btnGroupMap.end(); iter++) {
        iter.value()->setChecked(false);
    }

    m_btnGroupMap.value(index)->setChecked(true);
    m_stackLayout->setCurrentIndex(index);
    adaptWindowSize(m_scale);
    if (m_stackLayout->currentWidget() != m_searchWidget) {
        m_sheet->m_operation.sidebarIndex = index;
    }
    onHandleOpenSuccessDelay();
    qCDebug(appLog) << "SheetSidebar::onBtnClicked end";
}

void SheetSidebar::setBookMark(int index, int state)
{
    qCDebug(appLog) << "SheetSidebar::setBookMark start";
    if (m_bookmarkWidget) {
        qCDebug(appLog) << "Handling bookmark for index:" << index << "state:" << state;
        m_bookmarkWidget->handleBookMark(index, state);
    }
    if (m_thumbnailWidget) {
        qCDebug(appLog) << "Setting bookmark state for thumbnail widget";
        m_thumbnailWidget->setBookMarkState(index, state);
    }
    qCDebug(appLog) << "SheetSidebar::setBookMark end";
}

void SheetSidebar::setCurrentPage(int page)
{
    qCDebug(appLog) << "SheetSidebar::setCurrentPage start - page:" << page;
    if (m_thumbnailWidget) {
        qCDebug(appLog) << "Setting page for thumbnail widget";
        m_thumbnailWidget->handlePage(page - 1);
    }
    if (m_bookmarkWidget) {
        qCDebug(appLog) << "Setting page for bookmark widget";
        m_bookmarkWidget->handlePage(page - 1);
    }
    if (m_catalogWidget) {
        qCDebug(appLog) << "Setting page for catalog widget";
        m_catalogWidget->handlePage(page - 1);
    }
    qCDebug(appLog) << "SheetSidebar::setCurrentPage end";
}

void SheetSidebar::handleOpenSuccess()
{
    qCDebug(appLog) << "SheetSidebar::handleOpenSuccess start";
    m_bOpenDocOpenSuccess = true;
    this->setVisible(m_sheet->operation().sidebarVisible);
    int nId = qBound(0, m_sheet->operation().sidebarIndex, m_stackLayout->count() - 2);
    onBtnClicked(nId);
    qCDebug(appLog) << "SheetSidebar::handleOpenSuccess end";
}

void SheetSidebar::onHandleOpenSuccessDelay()
{
    qCDebug(appLog) << "SheetSidebar::onHandleOpenSuccessDelay start";
    if (m_bOpenDocOpenSuccess) {
        qCDebug(appLog) << "Document opened successfully, setting single shot timer";
        QTimer::singleShot(100, this, SLOT(onHandWidgetDocOpenSuccess()));
    }
    qCDebug(appLog) << "SheetSidebar::onHandleOpenSuccessDelay end";
}

void SheetSidebar::onHandWidgetDocOpenSuccess()
{
    qCDebug(appLog) << "SheetSidebar::onHandWidgetDocOpenSuccess start";
    if (!this->isVisible()) {
        qCDebug(appLog) << "Widget not visible, returning";
        return;
    }

    QWidget *curWidget = m_stackLayout->currentWidget();
    if (curWidget == m_thumbnailWidget) {
        qCDebug(appLog) << "Handling open success for thumbnail widget";
        m_thumbnailWidget->handleOpenSuccess();
    } else if (curWidget == m_catalogWidget) {
        qCDebug(appLog) << "Handling open success for catalog widget";
        m_catalogWidget->handleOpenSuccess();
    } else if (curWidget == m_bookmarkWidget) {
        qCDebug(appLog) << "Handling open success for bookmark widget";
        m_bookmarkWidget->handleOpenSuccess();
    } else if (curWidget == m_notesWidget) {
        qCDebug(appLog) << "Handling open success for notes widget";
        m_notesWidget->handleOpenSuccess();
    }
    qCDebug(appLog) << "SheetSidebar::onHandWidgetDocOpenSuccess end";
}

void SheetSidebar::handleSearchStart(const QString &text)
{
    qCDebug(appLog) << "Starting search for:" << text;
    for (auto iter = m_btnGroupMap.begin(); iter != m_btnGroupMap.end(); iter++) {
        iter.value()->setEnabled(false);
    }

    m_searchWidget->clearFindResult();
    m_searchWidget->searchKey(text);
    onBtnClicked(m_searchId);
    this->setVisible(true);
    qCDebug(appLog) << "Search UI activated";
}

void SheetSidebar::handleSearchStop()
{
    qCDebug(appLog) << "SheetSidebar::handleSearchStop start";
    for (auto iter = m_btnGroupMap.begin(); iter != m_btnGroupMap.end(); iter++) {
        iter.value()->setEnabled(true);
    }

    int nId = qBound(0, m_sheet->operation().sidebarIndex, m_stackLayout->count() - 1);
    onBtnClicked(nId);
    this->setVisible(m_sheet->operation().sidebarVisible);
    m_searchWidget->clearFindResult();
    qCDebug(appLog) << "SheetSidebar::handleSearchStop end";
}

void SheetSidebar::handleSearchResultComming(const deepin_reader::SearchResult &res)
{
    qCDebug(appLog) << "SheetSidebar::handleSearchResultComming start";
    m_searchWidget->handleSearchResultComming(res);
}

int SheetSidebar::handleFindFinished()
{
    qCDebug(appLog) << "SheetSidebar::handleFindFinished start";
    return m_searchWidget->handleFindFinished();
}

void SheetSidebar::handleRotate()
{
    qCDebug(appLog) << "SheetSidebar::handleRotate start";
    if (m_thumbnailWidget)
        m_thumbnailWidget->handleRotate();
    qCDebug(appLog) << "SheetSidebar::handleRotate end";
}

void SheetSidebar::handleAnntationMsg(const int &msg, int index, deepin_reader::Annotation *anno)
{
    qCDebug(appLog) << "SheetSidebar::handleAnntationMsg start";
    if (m_notesWidget)
        m_notesWidget->handleAnntationMsg(msg, anno);

    Q_UNUSED(index);
    qCDebug(appLog) << "SheetSidebar::handleAnntationMsg end";
}

DToolButton *SheetSidebar::createBtn(const QString &btnName, const QString &objName)
{
    qCDebug(appLog) << "Creating button:" << btnName;
    int tW = 36;
    auto btn = new DToolButton(this);
    btn->setToolTip(btnName);
    btn->setObjectName(objName);
    btn->setAccessibleName("Button_" + objName);
    btn->setFixedSize(QSize(tW, tW));
    btn->setIconSize(QSize(tW, tW));
    btn->setCheckable(true);
    if ("search" != objName) {
        btn->setFocusPolicy(Qt::TabFocus);
    }
    qCDebug(appLog) << "Button created:" << btnName;
    return btn;
}

void SheetSidebar::resizeEvent(QResizeEvent *event)
{
    // qCDebug(appLog) << "SheetSidebar::resizeEvent start - size:" << event->size();
    qreal scale = event->size().width() * 1.0 / LEFTMINWIDTH;
    adaptWindowSize(scale);
    BaseWidget::resizeEvent(event);
    // qCDebug(appLog) << "SheetSidebar::resizeEvent end";
}

void SheetSidebar::adaptWindowSize(const double &scale)
{
    qCDebug(appLog) << "Adapting window size with scale:" << scale;
    m_scale = scale;
    BaseWidget *curWidget = dynamic_cast<BaseWidget *>(m_stackLayout->currentWidget());
    if (curWidget) {
        qCDebug(appLog) << "Adapting widget:" << curWidget->metaObject()->className();
        curWidget->adaptWindowSize(scale);
    }
}

void SheetSidebar::keyPressEvent(QKeyEvent *event)
{
    // qCDebug(appLog) << "SheetSidebar::keyPressEvent start - key:" << event->key();
    QStringList pFilterList = QStringList() << Dr::key_pgUp << Dr::key_pgDown
                              << Dr::key_down << Dr::key_up
                              << Dr::key_left << Dr::key_right
                              << Dr::key_delete;

    QString key = Utils::getKeyshortcut(event);

    if (pFilterList.contains(key)) {
        // qCDebug(appLog) << "Processing filtered key:" << key;
        dealWithPressKey(key);
    }

    BaseWidget::keyPressEvent(event);
    // qCDebug(appLog) << "SheetSidebar::keyPressEvent end";
}

void SheetSidebar::showEvent(QShowEvent *event)
{
    // qCDebug(appLog) << "SheetSidebar::showEvent start";
    BaseWidget::showEvent(event);
    onHandleOpenSuccessDelay();
    // qCDebug(appLog) << "SheetSidebar::showEvent end";
}

void SheetSidebar::showMenu()
{
    qCDebug(appLog) << "Showing menu";
    DToolButton *bookmarkbtn = this->findChild<DToolButton *>("bookmark");
    if (m_bookmarkWidget && bookmarkbtn && bookmarkbtn->isChecked()) {
        m_bookmarkWidget->showMenu();
    }
    DToolButton *annotationbtn = this->findChild<DToolButton *>("annotation");
    if (m_notesWidget && annotationbtn && annotationbtn->isChecked()) {
        m_notesWidget->showMenu();
    }
    qCDebug(appLog) << "Menu shown";
}

void SheetSidebar::dealWithPressKey(const QString &sKey)
{
    qCDebug(appLog) << "Dealing with press key:" << sKey;
    if (sKey == Dr::key_up || sKey == Dr::key_left) {
        onJumpToPrevPage();
    } else if (sKey == Dr::key_pgUp) {
        onJumpToPageUp();
    } else if (sKey == Dr::key_down || sKey == Dr::key_right) {
        onJumpToNextPage();
    } else if (sKey == Dr::key_pgDown) {
        onJumpToPageDown();
    } else if (sKey == Dr::key_delete) {
        deleteItemByKey();
    }
    qCDebug(appLog) << "Dealing with press key end";
}

void SheetSidebar::onJumpToPrevPage()
{
    qCDebug(appLog) << "Jumping to previous page";
    QWidget *curWidget = m_stackLayout->currentWidget();
    qCDebug(appLog) << "Jumping to previous page in widget:" << (curWidget ? curWidget->metaObject()->className() : "null");
    if (curWidget == m_thumbnailWidget) {
        qCDebug(appLog) << "Jumping to previous page in thumbnail widget";
        m_thumbnailWidget->prevPage();
    }  else if (curWidget == m_bookmarkWidget) {
        qCDebug(appLog) << "Jumping to previous page in bookmark widget";
        m_bookmarkWidget->prevPage();
    } else if (curWidget == m_notesWidget) {
        qCDebug(appLog) << "Jumping to previous page in notes widget";
        m_notesWidget->prevPage();
    } else if (curWidget == m_catalogWidget) {
        qCDebug(appLog) << "Jumping to previous page in catalog widget";
        m_catalogWidget->prevPage();
    }
    qCDebug(appLog) << "Jumping to previous page end";
}

void SheetSidebar::onJumpToPageUp()
{
    qCDebug(appLog) << "Jumping to page up";
    QWidget *curWidget = m_stackLayout->currentWidget();
    if (curWidget == m_thumbnailWidget) {
        qCDebug(appLog) << "Jumping to page up in thumbnail widget";
        m_thumbnailWidget->pageUp();
    } else if (curWidget == m_bookmarkWidget) {
        qCDebug(appLog) << "Jumping to page up in bookmark widget";
        m_bookmarkWidget->pageUp();
    } else if (curWidget == m_notesWidget) {
        qCDebug(appLog) << "Jumping to page up in notes widget";
        m_notesWidget->pageUp();
    } else if (curWidget == m_catalogWidget) {
        qCDebug(appLog) << "Jumping to page up in catalog widget";
        m_catalogWidget->pageUp();
    }
    qCDebug(appLog) << "Jumping to page up end";
}

void SheetSidebar::onJumpToNextPage()
{
    qCDebug(appLog) << "Jumping to next page";
    QWidget *curWidget = m_stackLayout->currentWidget();
    if (curWidget == m_thumbnailWidget) {
        qCDebug(appLog) << "Jumping to next page in thumbnail widget";
        m_thumbnailWidget->nextPage();
    }  else if (curWidget == m_bookmarkWidget) {
        qCDebug(appLog) << "Jumping to next page in bookmark widget";
        m_bookmarkWidget->nextPage();
    } else if (curWidget == m_notesWidget) {
        qCDebug(appLog) << "Jumping to next page in notes widget";
        m_notesWidget->nextPage();
    } else if (curWidget == m_catalogWidget) {
        qCDebug(appLog) << "Jumping to next page in catalog widget";
        m_catalogWidget->nextPage();
    }
    qCDebug(appLog) << "Jumping to next page end";
}

void SheetSidebar::onJumpToPageDown()
{
    qCDebug(appLog) << "Jumping to page down";
    QWidget *curWidget = m_stackLayout->currentWidget();
    if (curWidget == m_thumbnailWidget) {
        qCDebug(appLog) << "Jumping to page down in thumbnail widget";
        m_thumbnailWidget->pageDown();
    } else if (curWidget == m_bookmarkWidget) {
        qCDebug(appLog) << "Jumping to page down in bookmark widget";
        m_bookmarkWidget->pageDown();
    } else if (curWidget == m_notesWidget) {
        qCDebug(appLog) << "Jumping to page down in notes widget";
        m_notesWidget->pageDown();
    } else if (curWidget == m_catalogWidget) {
        qCDebug(appLog) << "Jumping to page down in catalog widget";
        m_catalogWidget->pageDown();
    }
    qCDebug(appLog) << "Jumping to page down end";
}

void SheetSidebar::deleteItemByKey()
{
    qCDebug(appLog) << "Deleting item by key";
    QWidget *widget = m_stackLayout->currentWidget();
    if (widget == m_bookmarkWidget) {
        qCDebug(appLog) << "Deleting item by key in bookmark widget";
        m_bookmarkWidget->deleteItemByKey();
    } else if (widget == m_notesWidget) {
        qCDebug(appLog) << "Deleting item by key in notes widget";
        m_notesWidget->deleteItemByKey();
    }
    qCDebug(appLog) << "Deleting item by key end";
}

bool SheetSidebar::event(QEvent *event)
{
    // qCDebug(appLog) << "SheetSidebar::event start - type:" << event->type();
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
        if (key_event->key() == Qt::Key_Menu && !key_event->isAutoRepeat()) {
            // qCDebug(appLog) << "Key_Menu pressed, showing menu";
            showMenu();
        }
        if (key_event->key() == Qt::Key_M && (key_event->modifiers() & Qt::AltModifier) && !key_event->isAutoRepeat()) {
            // qCDebug(appLog) << "Alt+M pressed, showing menu";
            showMenu();
        }
    }

    // qCDebug(appLog) << "SheetSidebar::event end";
    return BaseWidget::event(event);
}

void SheetSidebar::onUpdateWidgetTheme()
{
    qCDebug(appLog) << "Updating widget theme";
    updateWidgetTheme();
    for (auto iter = m_btnGroupMap.begin(); iter != m_btnGroupMap.end(); iter++) {
        const QString &objName = iter.value()->objectName();
        const QIcon &icon = QIcon::fromTheme(QString("dr_") + objName);
        iter.value()->setIcon(icon);
        // qCDebug(appLog) << "Updated icon for button:" << objName;
    }
}

void SheetSidebar::changeResetModelData()
{
    // qCDebug(appLog) << "Changing reset model data";
    if (m_notesWidget) {
        // qCDebug(appLog) << "Changing reset model data in notes widget";
        m_notesWidget->changeResetModelData();
    }
    // qCDebug(appLog) << "Changing reset model data end";
}
