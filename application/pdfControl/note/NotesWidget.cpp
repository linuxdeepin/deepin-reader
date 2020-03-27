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

#include "business/AppInfo.h"
#include "docview/docummentproxy.h"

#include "widgets/main/MainTabWidgetEx.h"

NotesWidget::NotesWidget(DWidget *parent)
    : CustomWidget(NOTE_WIDGET, parent)
{
    initWidget();

    initConnection();

    m_ThreadLoadImage.setParentWidget(this);

    dApp->m_pModelService->addObserver(this);
}

NotesWidget::~NotesWidget()
{
    m_ThreadLoadImage.stopThreadRun();
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
    auto curItem = m_pNotesList->currentItem();
    if (curItem == nullptr)
        return;

    auto t_widget = qobject_cast<NotesItemWidget *>(m_pNotesList->itemWidget(curItem));
    if (t_widget) {
        if (t_widget->bSelect()) {
            int nType = t_widget->nNoteType();

            QString t_uuid = t_widget->noteUUId();
            int page = t_widget->nPageIndex();
            QString sContent = t_uuid + Constant::sQStringSep + QString::number(page);

            if (nType == NOTE_HIGHLIGHT) {
                emit sigDeleteContent(MSG_NOTE_DELETE_CONTENT, sContent);
            } else {
                emit sigDeleteContent(MSG_NOTE_PAGE_DELETE_CONTENT, sContent);
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
    m_pNotesList->setListType(E_NOTE_WIDGET);
    connect(m_pNotesList, SIGNAL(sigSelectItem(QListWidgetItem *)), SLOT(slotSelectItem(QListWidgetItem *)));
    connect(m_pNotesList, SIGNAL(sigListMenuClick(const int &)), SLOT(slotListMenuClick(const int &)));
    int tW = 170;
    int tH = 36;
    dApp->adaptScreenView(tW, tH);
    m_pAddAnnotationBtn = new DPushButton(this);
    m_pAddAnnotationBtn->setFixedHeight(tH);
    m_pAddAnnotationBtn->setMinimumWidth(tW);
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
            auto t_widget = qobject_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
            if (t_widget) {
                QString t_uuid = t_widget->noteUUId();
                if (t_uuid == sUuid) {
                    int t_nPage = t_widget->nPageIndex();
                    emit sigUpdateThumbnail(t_nPage);

                    t_widget->deleteLater();
                    t_widget = nullptr;

                    delete pItem;
                    pItem = nullptr;

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
        QString sUuid = sList.at(0);
        QString sText = sList.at(1);
        QString sPage = sList.at(2);

        bool rl = false;

        int iCount = m_pNotesList->count();
        for (int iLoop = 0; iLoop < iCount; iLoop++) {
            auto pItem = m_pNotesList->item(iLoop);
            if (pItem) {
                auto t_widget = qobject_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
                if (t_widget) {
                    if (t_widget->nPageIndex() == sPage.toInt() && t_widget->noteUUId() == sUuid) {
                        clearItemColor();

                        rl = true;
                        t_widget->setBSelect(true);
                        t_widget->setTextEditText(sText);

                        m_pNotesList->setCurrentItem(pItem);

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


void NotesWidget::slotOpenFileOk(const QString &sPath)
{
    m_strBindPath = sPath;

    m_nIndex = 0;
    m_ThreadLoadImage.setIsLoaded(false);
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }

    MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
    DocummentProxy *t_docPtr = pMtwe->getCurFileAndProxy(m_strBindPath);
    if (t_docPtr) {
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

        m_ThreadLoadImage.setListNoteSt(list_note);
        m_ThreadLoadImage.setIsLoaded(true);
        if (!m_ThreadLoadImage.isRunning()) {
            m_ThreadLoadImage.start();
        }
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

void NotesWidget::slotListMenuClick(const int &iAction)
{
    if (iAction == E_NOTE_COPY) {
        CopyNoteContent();
    } else if (iAction == E_NOTE_DELETE) {
        DeleteItemByKey();
    }
}

void NotesWidget::slotSelectItem(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    setSelectItemBackColor(item);

    auto t_widget = qobject_cast<NotesItemWidget *>(m_pNotesList->itemWidget(item));
    if (t_widget) {
        QString t_uuid = t_widget->noteUUId();
        int page = t_widget->nPageIndex();

        auto pDocProxy = MainTabWidgetEx::Instance()->getCurFileAndProxy(m_strBindPath);
        if (pDocProxy) {
            pDocProxy->jumpToHighLight(t_uuid, page);
        }
    }
}

void NotesWidget::SlotRightDeleteItem()
{
    DeleteItemByKey();
}

void NotesWidget::__JumpToPrevItem()
{
    if (m_pNotesList == nullptr) {
        return;
    }
    if (m_pNotesList->count() <= 0) {
        return;
    }

    auto current_item = m_pNotesList->currentItem();
    if (current_item) {
        int t_index = m_pNotesList->row(current_item);
        t_index--;
        if (t_index < 0)
            return;

        auto item = m_pNotesList->item(t_index);
        if (item == nullptr)
            return;

        slotSelectItem(item);
    }
}

void NotesWidget::__JumpToNextItem()
{
    if (m_pNotesList == nullptr) {
        return;
    }

    if (m_pNotesList->count() <= 0) {
        return;
    }

    auto current_item = m_pNotesList->currentItem();
    if (current_item) {
        int t_index = m_pNotesList->row(current_item);
        t_index++;
        if (t_index < 0)
            return;

        auto item = m_pNotesList->item(t_index);
        if (item == nullptr)
            return;

        slotSelectItem(item);
    }
}

/**
 * @brief NotesWidget::slotAddAnnotation
 * 添加注释按钮
 */
void NotesWidget::slotAddAnnotation()
{
    MainTabWidgetEx::Instance()->setCurrentState(NOTE_ADD_State);
}

void NotesWidget::SlotAnntationMsg(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_NOTE_ADD_ITEM) {
        __AddNoteItem(msgContent);
    } else if (msgType == MSG_NOTE_DELETE_ITEM || msgType == MSG_NOTE_PAGE_DELETE_ITEM) {
        __DeleteNoteItem(msgContent);
    } else if (msgType == MSG_NOTE_UPDATE_ITEM || msgType == MSG_NOTE_PAGE_UPDATE_ITEM) {
        __UpdateNoteItem(msgContent);
    } else if (msgType == MSG_NOTE_PAGE_ADD_ITEM) {
        __AddNoteItem(msgContent, NOTE_ICON);
    }
}

void NotesWidget::CopyNoteContent()
{
    auto curItem = m_pNotesList->currentItem();
    if (curItem == nullptr)
        return;

    auto t_widget = qobject_cast<NotesItemWidget *>(m_pNotesList->itemWidget(curItem));
    if (t_widget) {
        t_widget->CopyItemText();
    }
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

        auto dproxy = MainTabWidgetEx::Instance()->getCurFileAndProxy(m_strBindPath);
        if (nullptr == dproxy) {
            return;
        }
        int tW = 48;
        int tH = 68;
        dApp->adaptScreenView(tW, tH);
        QImage image;
        bool rl = dproxy->getImage(t_nPage, image, tW, tH /*42, 62*/);
        if (rl) {
            QImage img = Utils::roundImage(QPixmap::fromImage(image), ICON_SMALL);
            auto item = addNewItem(img, t_nPage, t_strUUid, t_strText, true, iType);
            if (item) {
                m_pNotesList->setCurrentItem(item);
                emit sigUpdateThumbnail(t_nPage);
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
    connect(&m_ThreadLoadImage, SIGNAL(sigLoadImage(const QImage &)), SLOT(slotLoadImage(const QImage &)));
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

    auto t_widget = qobject_cast<NotesItemWidget *>(m_pNotesList->itemWidget(item));
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
        auto pItemWidget = qobject_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pCurItem));
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
        int tW = LEFTMINWIDTH;
        int tH = 80;
        dApp->adaptScreenView(tW, tH);
        itemWidget->setMinimumSize(QSize(tW, tH));
        itemWidget->setBSelect(bNew);

        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(tW, tH));

        m_pNotesList->setItemWidget(item, itemWidget);

        return item;
    }

    return nullptr;
}

NotesItemWidget *NotesWidget::getItemWidget(QListWidgetItem *item)
{
    if (m_pNotesList == nullptr) {
        return nullptr;
    }
    auto pWidget = qobject_cast<NotesItemWidget *>(m_pNotesList->itemWidget(item));
    if (pWidget) {
        return pWidget;
    }
    return nullptr;
}

/**
 * @brief NotesWidget::adaptWindowSize
 * 注释缩略图自适应视窗大小
 * @param scale  缩放因子 大于0的数
 */
void NotesWidget::adaptWindowSize(const double &scale)
{
    double width = 1.0;
    double height = 1.0;

    //set item size
    width = static_cast<double>(LEFTMINWIDTH) * scale;
    height = static_cast<double>(80) * scale;

    if (m_pNotesList) {
        int itemCount = 0;
        itemCount = m_pNotesList->count();
        if (itemCount > 0) {
            for (int index = 0; index < itemCount; index++) {
                auto item = m_pNotesList->item(index);
                if (item) {
                    auto itemWidget = getItemWidget(item);
                    if (itemWidget) {
                        item->setSizeHint(QSize(static_cast<int>(width), static_cast<int>(height)));
                        itemWidget->adaptWindowSize(scale);
                    }
                }
            }
        }
    }
}

/**
 * @brief NotesWidget::updateThumbnail
 * 高亮操作之后要跟换相应的缩略图
 * @param page 页码数，从0开始
 */
void NotesWidget::updateThumbnail(const int &page)
{
    if (m_pNotesList == nullptr) {
        return;
    }
    int itemNum = 0;
    itemNum = m_pNotesList->count();
    if (itemNum <= 0) {
        return;
    }
    QImage image;
    int tW = 48;
    int tH = 68;
    dApp->adaptScreenView(tW, tH);
    MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
    auto dproxy = pMtwe->getCurFileAndProxy(m_strBindPath);
    dproxy->getImage(page, image, tW, tH);
    for (int index = 0; index < itemNum; index++) {
        auto item = m_pNotesList->item(index);
        if (item) {
            auto itemWidget = getItemWidget(item);
            if (itemWidget) {
                if (itemWidget->nPageIndex() == page) {
                    if (nullptr == dproxy) {
                        return;
                    }
                    itemWidget->setLabelImage(image);
                }
            }
        }
    }
}

QString NotesWidget::getBindPath() const
{
    return m_strBindPath;
}

/**
 * @brief NotesWidget::dealWithData
 * 处理全局信号函数
 * @return
 */
int NotesWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        slotOpenFileOk(msgContent);
    }

    if (m_pMsgList.contains(msgType)) {
        return MSG_OK;
    }
    return MSG_NO_OK;
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
            auto dproxy = MainTabWidgetEx::Instance()->getCurFileAndProxy(m_pNoteWidget->getBindPath());
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
