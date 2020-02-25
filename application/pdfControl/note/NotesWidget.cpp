/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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
#include "NotesWidget.h"

#include "NotesItemWidget.h"

#include "controller/DataManager.h"
#include "docview/docummentproxy.h"

NotesWidget::NotesWidget(DWidget *parent)
    : CustomWidget("NotesWidget", parent)
{
    m_pMsgList = {MSG_NOTE_ADD_ITEM, MSG_NOTE_DELETE_ITEM, MSG_NOTE_UPDATE_ITEM,
                  MSG_NOTE_SELECT_ITEM,
                  MSG_NOTE_PAGE_ADD_ITEM, MSG_NOTE_PAGE_DELETE_ITEM, MSG_NOTE_PAGE_UPDATE_ITEM
                 };
    initWidget();

    initConnection();

    dApp->m_pModelService->addObserver(this);
}

NotesWidget::~NotesWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

/**
 * @brief NotesWidget::prevPage
 * 上一页
 */
void NotesWidget::prevPage()
{
    __JumpToPrevItem();
}

/**
 * @brief NotesWidget::nextPage
 * 下一页
 */
void NotesWidget::nextPage()
{
    __JumpToNextItem();
}

void NotesWidget::DeleteItemByKey()
{
    bool bFocus = this->hasFocus();
    if (bFocus) {
        auto curItem = m_pNotesList->currentItem();
        if (curItem == nullptr)
            return;

        auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(curItem));
        if (t_widget) {
            if (t_widget->bSelect()) {
                int nType = t_widget->nNoteType();

                QString t_uuid = t_widget->noteUUId();
                int page = t_widget->nPageIndex();
                QString sContent = t_uuid + Constant::sQStringSep + QString::number(page);

                if (nType == NOTE_HIGHLIGHT) {
                    notifyMsg(MSG_NOTE_DELETE_CONTENT, sContent);
                } else {
                    notifyMsg(MSG_NOTE_PAGE_DELETE_CONTENT, sContent);
                }
            }
        }
    }
}

/**
 * @brief NotesWidget::initWidget
 * 初始化界面
 */
void NotesWidget::initWidget()
{
    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 8, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    m_pNotesList = new CustomListWidget;

    m_pAddAnnotationBtn = new DPushButton(this);
    m_pAddAnnotationBtn->setFixedHeight(36);
    m_pAddAnnotationBtn->setMinimumWidth(170);
    m_pAddAnnotationBtn->setText(tr("Add annotation"));
    DFontSizeManager::instance()->bind(m_pAddAnnotationBtn, DFontSizeManager::T6);
    connect(m_pAddAnnotationBtn, SIGNAL(clicked()), this, SLOT(slotAddAnnotation()));

    auto m_pHBoxLayout = new QHBoxLayout;
    m_pHBoxLayout->setContentsMargins(10, 6, 10, 6);
    m_pVLayout->addWidget(m_pNotesList);
    m_pVLayout->addWidget(new DHorizontalLine(this));
    m_pHBoxLayout->addWidget(m_pAddAnnotationBtn);
    m_pVLayout->addItem(m_pHBoxLayout);
}

void NotesWidget::slotDealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_NOTE_ADD_ITEM == msgType) {
        __AddNoteItem(msgContent);
    } else if (MSG_NOTE_PAGE_ADD_ITEM == msgType) {
        __AddNoteItem(msgContent, NOTE_PAGE);
    } else if (MSG_NOTE_DELETE_ITEM == msgType || MSG_NOTE_PAGE_DELETE_ITEM == msgType) {
        __DeleteNoteItem(msgContent);
    } else if (MSG_NOTE_UPDATE_ITEM == msgType || MSG_NOTE_PAGE_UPDATE_ITEM == msgType) {
        __UpdateNoteItem(msgContent);
    } else if (MSG_NOTE_SELECT_ITEM == msgType) {
        __RightSelectItem(msgContent);
    }
}

/**
 * @brief NotesWidget::slotAddNoteItem
 * 增加注释缩略图Item
 */
void NotesWidget::__AddNoteItem(const QString &note, const int &iType)
{
    clearItemColor();
    addNotesItem(note, iType);
}

/**
 * @brief NotesWidget::__DeleteNoteItem
 *       根据 uuid    删除注释item
 */
void NotesWidget::__DeleteNoteItem(const QString &sUuid)
{
    if (m_pNotesList == nullptr) {
        return;
    }

    for (int row = 0; row < m_pNotesList->count(); ++row) {
        auto pItem = m_pNotesList->item(row);
        if (pItem) {
            auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
            if (t_widget) {
                QString t_uuid = t_widget->noteUUId();
                if (t_uuid == sUuid) {
                    delete t_widget;
                    t_widget = nullptr;

                    delete pItem;
                    pItem = nullptr;

                    // remove date from map and notify kong yun zhen
                    m_mapUuidAndPage.remove(sUuid);

                    notifyMsg(CENTRAL_SHOW_TIP, tr("The annotation has been removed"));
                    break;
                }
            }
        }
    }
}

void NotesWidget::__UpdateNoteItem(const QString &msgContent)
{
    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 3) {
        QString sText = sList.at(0);
        QString sUuid = sList.at(1);
        QString sPage = sList.at(2);

        bool rl = false;

        int iCount = m_pNotesList->count();
        for (int iLoop = 0; iLoop < iCount; iLoop++) {
            auto pItem = m_pNotesList->item(iLoop);
            if (pItem) {
                auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
                if (t_widget) {
                    if (t_widget->nPageIndex() == sPage.toInt() && t_widget->noteUUId() == sUuid) {
                        rl = true;
                        t_widget->setBSelect(true);
                        t_widget->setTextEditText(sText);
                        break;
                    }
                }
            }
        }

        if (!rl) {
            QString sContent = sUuid + Constant::sQStringSep +
                               sText + Constant::sQStringSep +
                               sPage;

            __AddNoteItem(sContent);
        }
    }
}


void NotesWidget::slotOpenFileOk()
{
    m_nIndex = 0;
    m_ThreadLoadImage.setIsLoaded(false);
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }

    auto t_docPtr = DocummentProxy::instance();
    if (!t_docPtr) {
        return;
    }

    m_mapUuidAndPage.clear();
    m_pNotesList->clear();

    QList<stHighlightContent> list_note;
    t_docPtr->getAllAnnotation(list_note);

    if (list_note.count() < 1) {
        return;
    }

    for (int index = 0; index < list_note.count(); ++index) {
        stHighlightContent st = list_note.at(index);
        if (st.strcontents == QString("")) {
            continue;
        }

        int page = static_cast<int>(st.ipage);
        QString uuid = st.struuid;
        QString contant = st.strcontents;

        addNewItem(QImage(), page, uuid, contant);
    }

    m_pNotesList->setCurrentRow(0);

    m_ThreadLoadImage.setListNoteSt(list_note);
    m_ThreadLoadImage.setIsLoaded(true);
    if (!m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.start();
    }
}

void NotesWidget::slotCloseFile()
{
    m_nIndex = 0;
    m_ThreadLoadImage.setIsLoaded(false);
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }

    m_mapUuidAndPage.clear();
    if (m_pNotesList) {
        m_pNotesList->clear();
    }
}

void NotesWidget::slotLoadImage(const QImage &image)
{
    if (m_pNotesList->count() < 1 || m_nIndex >= m_pNotesList->count()) {
        return;
    }

    QListWidgetItem *pItem = m_pNotesList->item(m_nIndex);
    if (pItem) {
        NotesItemWidget *t_widget =
            reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
        if (t_widget) {
            t_widget->setLabelImage(image);
        }
    }
    ++m_nIndex;
}

void NotesWidget::slotSelectItem(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    setSelectItemBackColor(item);

    auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(item));
    if (t_widget) {
        QString t_uuid = t_widget->noteUUId();
        int page = t_widget->nPageIndex();

        auto pDocProxy = DocummentProxy::instance();
        if (pDocProxy) {
            pDocProxy->jumpToHighLight(t_uuid, page);
        }
    }
}

void NotesWidget::__JumpToPrevItem()
{
    if (m_pNotesList == nullptr ||
            DataManager::instance()->bThumbnIsShow() == false ||
            DataManager::instance()->CurShowState() != FILE_NORMAL) {
        return;
    }
    if (m_pNotesList->count() <= 0) {
        return;
    }
    int t_index = -1;

    auto current_item = m_pNotesList->currentItem();
    if (current_item) {
        t_index = m_pNotesList->row(current_item);
        if (--t_index < 0)
            return;
        auto item = m_pNotesList->item(t_index);
        if (item == nullptr)
            return;

        slotSelectItem(item);
    }
}

void NotesWidget::__JumpToNextItem()
{
    if (m_pNotesList == nullptr ||
            DataManager::instance()->bThumbnIsShow() == false ||
            DataManager::instance()->CurShowState() != FILE_NORMAL) {
        return;
    }

    if (m_pNotesList->count() <= 0) {
        return;
    }

    int t_index = -1;
    auto current_item = m_pNotesList->currentItem();
    if (current_item) {
        t_index = m_pNotesList->row(current_item);
        if (++t_index < 0)
            return;
        auto item = m_pNotesList->item(t_index);
        if (item == nullptr)
            return;

        slotSelectItem(item);
    }
}

void NotesWidget::__RightSelectItem(const QString &uuid)
{
    if (m_pNotesList == nullptr) {
        return;
    }

    for (int index = 0; index < m_pNotesList->count(); ++index) {
        auto item = m_pNotesList->item(index);
        auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(item));
        if (t_widget) {
            if (t_widget->noteUUId() == uuid) {
                slotSelectItem(item);
            }
        }
    }
}

/**
 * @brief NotesWidget::slotAddAnnotation
 * 添加注释按钮
 */
void NotesWidget::slotAddAnnotation()
{
    notifyMsg(MSG_NOTE_PAGE_ADD);
}

/**
 * @brief NotesWidget::addNotesItem
 * 给新节点填充内容（包括文字、缩略图等内容）
 * @param image
 * @param page
 * @param text
 */
void NotesWidget::addNotesItem(const QString &text, const int &iType)
{
    QStringList t_strList = text.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (t_strList.count() == 3) {
        QString t_strUUid = t_strList.at(0).trimmed();
        QString t_strText = t_strList.at(1).trimmed();
        int t_nPage = t_strList.at(2).trimmed().toInt();

        auto dproxy = DocummentProxy::instance();
        if (nullptr == dproxy) {
            return;
        }
        QImage image;
        bool rl = dproxy->getImage(t_nPage, image, 48, 68 /*42, 62*/);
        if (rl) {
            QImage img = Utils::roundImage(QPixmap::fromImage(image), ICON_SMALL);
            auto item = addNewItem(img, t_nPage, t_strUUid, t_strText, true, iType);
            if (item) {
                m_mapUuidAndPage.insert(t_strUUid, t_nPage);
            }
        }
    }
}

/**
 * @brief NotesWidget::initConnection
 * 初始化信号槽
 */
void NotesWidget::initConnection()
{
    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)), SLOT(slotDealWithData(const int &, const QString &)));
    connect(this, SIGNAL(sigOpenFileOk()), SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigCloseFile()), SLOT(slotCloseFile()));

    connect(&m_ThreadLoadImage, SIGNAL(sigLoadImage(const QImage &)), SLOT(slotLoadImage(const QImage &)));

    connect(m_pNotesList, SIGNAL(sigSelectItem(QListWidgetItem *)), SLOT(slotSelectItem(QListWidgetItem *)));
}

/**
 * @brief BookMarkWidget::setSelectItemBackColor
 * 绘制选中外边框,蓝色
 */
void NotesWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    clearItemColor();

    m_pNotesList->setCurrentItem(item);

    auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(item));
    if (t_widget) {
        t_widget->setBSelect(true);
    }
}

void NotesWidget::clearItemColor()
{
    if (m_pNotesList == nullptr)
        return;
    auto pCurItem = m_pNotesList->currentItem();
    if (pCurItem) {
        auto pItemWidget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pCurItem));
        if (pItemWidget) {
            pItemWidget->setBSelect(false);
        }
    }
}

/**
 * @brief NotesWidget::addNewItem
 * 增加新的注释item
 * @param image
 * @param page
 * @param uuid
 * @param text
 */
QListWidgetItem *NotesWidget::addNewItem(const QImage &image, const int &page, const QString &uuid, const QString &text,
                                         const bool &bNew, const int &iType)
{
    auto item = m_pNotesList->insertWidgetItem(page);
    if (item) {
        auto itemWidget = new NotesItemWidget(this);
        itemWidget->setNNoteType(iType);
        itemWidget->setLabelImage(image);
        itemWidget->setNoteUUid(uuid);
        itemWidget->setStrPage(QString::number(page));
        itemWidget->setLabelPage(page, 1);
        itemWidget->setTextEditText(text);
        itemWidget->setMinimumSize(QSize(LEFTMINWIDTH, 80));
        itemWidget->setBSelect(bNew);

        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(LEFTMINWIDTH, 80));

        m_pNotesList->setItemWidget(item, itemWidget);
        m_pNotesList->setCurrentItem(item);

        return item;
    }

    return nullptr;
}

/**
 * @brief NotesWidget::dealWithData
 * 处理全局信号函数
 * @return
 */
int NotesWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return ConstantMsg::g_effective_res;;
    }
    if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        emit sigOpenFileOk();
    } else if (MSG_CLOSE_FILE == msgType) {
        emit sigCloseFile();
    }

    return 0;
}

/*********************class ThreadLoadImageOfNote***********************/
/**********************加载注释列表***************************************/

ThreadLoadImageOfNote::ThreadLoadImageOfNote(QObject *parent)
    : QThread(parent)
{
}

void ThreadLoadImageOfNote::stopThreadRun()
{
    m_isLoaded = false;

    quit();
    wait();
}

void ThreadLoadImageOfNote::run()
{
    while (m_isLoaded) {
        msleep(50);

        int t_page = -1;
        QImage image;
        bool bl = false;

        for (int page = 0; page < m_stListNote.count(); page++) {
            if (!m_isLoaded)
                break;
            auto dproxy = DocummentProxy::instance();
            if (nullptr == dproxy) {
                break;
            }

            stHighlightContent highContent = m_stListNote.at(page);

            if (highContent.strcontents == QString("")) {
                continue;
            }

            if (t_page != static_cast<int>(highContent.ipage)) {
                t_page = static_cast<int>(highContent.ipage);
                bl = dproxy->getImage(t_page, image, 48, 68 /*42, 62*/);
            }
            if (bl) {
                emit sigLoadImage(image);
            }
            msleep(50);
        }

        m_isLoaded = false;
        m_stListNote.clear();
        break;
    }
}
