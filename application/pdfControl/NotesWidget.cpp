#include "NotesWidget.h"
#include "controller/DataManager.h"
#include "frame/DocummentFileHelper.h"

NotesWidget::NotesWidget(CustomWidget *parent) :
    CustomWidget(QString("NotesWidget"), parent)
{
    initWidget();

    initConnection();
}

/**
 * @brief NotesWidget::initWidget
 * 初始化界面
 */
void NotesWidget::initWidget()
{
    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    m_pNotesList = new CustomListWidget;
    m_pNotesList->setSpacing(1);

    m_pVLayout->addWidget(m_pNotesList);
}

/**
 * @brief NotesWidget::slotAddNoteItem
 * 增加注释缩略图Item槽函数
 */
void NotesWidget::slotAddNoteItem(const QString &note)
{
    addNotesItem(note);
}

/**
 * @brief NotesWidget::slotDltNoteItem
 * 右键删除当前注释item
 */
void NotesWidget::slotDltNoteItem(QString uuid)
{
    for (int row = 0; row < m_pNotesList->count(); ++row) {
        auto pItem = m_pNotesList->item(row);
        if (pItem) {
            auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
            if (t_widget) {
                QString t_uuid = t_widget->noteUUId();
                if (t_uuid == uuid) {
                    int page = t_widget->nPageIndex();

                    delete t_widget;
                    t_widget = nullptr;

                    delete  pItem;
                    pItem = nullptr;

                    // remove date from map and notify kong yun zhen
                    m_mapUuidAndPage.remove(uuid);

                    auto dproxy = DocummentFileHelper::instance();
                    if (dproxy) {
                        dproxy->removeAnnotation(uuid, page);
                    }

                    break;
                }
            }
        }
    }
}

/**
 * @brief NotesWidget::slotDltNoteContant
 * 根据uuid删除本地缓存中的注释内容
 * @param uuid
 */
void NotesWidget::slotDltNoteContant(QString uuid)
{
    m_mapUuidAndPage.remove(uuid);
}

void NotesWidget::slotOpenFileOk()
{
    m_nIndex = 0;
    m_ThreadLoadImage.setIsLoaded(false);
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }

    auto t_docPtr = DocummentFileHelper::instance();
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
        addNewItem(st);
    }

    m_pNoteItem = m_pNotesList->item(0);

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
        m_pNoteItem = nullptr;
    }
}

void NotesWidget::slotLoadImage(const QImage &image)
{
    if (m_pNotesList->count() < 1 || m_nIndex >= m_pNotesList->count()) {
        return;
    }

    QListWidgetItem *pItem = m_pNotesList->item(m_nIndex);
    if (pItem) {
        NotesItemWidget *t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
        if (t_widget) {
            t_widget->setLabelImage(image);
        }
    }
    ++m_nIndex;
}

//  按 键盘 Del 删除
void NotesWidget::slotDelNoteItem()
{
    bool bFocus = this->hasFocus();
    if (bFocus) {
        auto curItem = m_pNotesList->currentItem();
        if (curItem == nullptr)
            return;

        auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(curItem));
        if (t_widget) {
            QString t_uuid = t_widget->noteUUId();
            int page  = t_widget->nPageIndex();

            delete t_widget;
            t_widget = nullptr;

            delete  curItem;
            curItem = nullptr;

            m_pNotesList->update();

            // remove date from map and notify kong yun zhen
            m_mapUuidAndPage.remove(t_uuid);

            auto t_pDocummentProxy = DocummentFileHelper::instance();
            if (t_pDocummentProxy) {
                t_pDocummentProxy->removeAnnotation(t_uuid, page);
            }
        }
        if (m_pNotesList->count() > 0) {
            m_pNoteItem = m_pNotesList->item(0);
        }
    }
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
        int page  = t_widget->nPageIndex();

        auto pDocProxy = DocummentFileHelper::instance();
        if (pDocProxy) {
            pDocProxy->jumpToHighLight(t_uuid, page);
        }
    }
}

void NotesWidget::slotJumpToPrevItem()
{
    if (DataManager::instance()->currentWidget() != WIDGET_NOTE || m_pNotesList == nullptr) {
        return;
    }

    if (m_pNotesList->count() <= 0) {
        return;
    }

    int t_index = -1;
    if (m_pNoteItem) {
        t_index = m_pNotesList->row(m_pNoteItem);
        if (--t_index < 0) return;
        auto item = m_pNotesList->item(t_index);
        if (item == nullptr) return;
        auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(item));
        if (t_widget) {
            clearItemColor();
            m_pNotesList->setCurrentItem(item);
            DocummentFileHelper::instance()->pageJump(t_widget->nPageIndex());
            m_pNoteItem = item;
            t_widget->setBSelect(true);
        }
    }
}

void NotesWidget::slotJumpToNextItem()
{
    if (DataManager::instance()->currentWidget() != WIDGET_NOTE || m_pNotesList == nullptr) {
        return;
    }

    if (m_pNotesList->count() <= 0) {
        return;
    }

    int t_index = -1;
    if (m_pNoteItem) {
        t_index = m_pNotesList->row(m_pNoteItem);
        if (++t_index < 0) return;
        auto item = m_pNotesList->item(t_index);
        if (item == nullptr) return;
        auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(item));
        if (t_widget) {
            clearItemColor();
            m_pNotesList->setCurrentItem(item);
            DocummentFileHelper::instance()->pageJump(t_widget->nPageIndex());
            m_pNoteItem = item;
            t_widget->setBSelect(true);
        }
    }
}


/**
 * @brief NotesWidget::addNotesItem
 * 给新节点填充内容（包括文字、缩略图等内容）
 * @param image
 * @param page
 * @param text
 */
void NotesWidget::addNotesItem(const QString &text)
{
    QStringList t_strList = text.split(QString("%"));
    if (t_strList.count() == 3) {
        QString t_strUUid = t_strList.at(0).trimmed();
        QString t_strText = t_strList.at(1).trimmed();
        int t_nPage = t_strList.at(2).trimmed().toInt();

        bool b_has = m_mapUuidAndPage.contains(t_strUUid);
        if (b_has) {
            flushNoteItemText(t_nPage, t_strUUid, t_strText);
        } else {
            auto dproxy = DocummentFileHelper::instance();
            if (nullptr == dproxy) {
                return;
            }
            QImage image;
            bool rl = dproxy->getImage(t_nPage, image, 34, 54/*28, 48*/);
            if (rl) {
                addNewItem(image, t_nPage, t_strUUid, t_strText);

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
    connect(this, SIGNAL(sigDltNoteItem(QString)), this, SLOT(slotDltNoteItem(QString)));
    connect(this, SIGNAL(sigDltNoteContant(QString)), this, SLOT(slotDltNoteContant(QString)));

    connect(this, SIGNAL(sigAddNewNoteItem(const QString &)), this, SLOT(slotAddNoteItem(const QString &)));

    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));
    connect(&m_ThreadLoadImage, SIGNAL(sigLoadImage(const QImage &)),
            this, SLOT(slotLoadImage(const QImage &)));
    connect(this, SIGNAL(sigDelNoteItem()), this, SLOT(slotDelNoteItem()));
    connect(m_pNotesList, SIGNAL(sigSelectItem(QListWidgetItem *)), this, SLOT(slotSelectItem(QListWidgetItem *)));
    connect(this, SIGNAL(sigJumpToPrevItem()), this, SLOT(slotJumpToPrevItem()));
    connect(this, SIGNAL(sigJumpToNextItem()), this, SLOT(slotJumpToNextItem()));
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

//    auto t_widget1 = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(m_pNoteItem));
//    if (t_widget1) {
//        t_widget1->setBSelect(false);
//    }
    clearItemColor();

    m_pNotesList->setCurrentItem(item);

    auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(item));
    if (t_widget) {
        t_widget->setBSelect(true);
        m_pNoteItem = item;
    }
}

void NotesWidget::clearItemColor()
{
    if (m_pNotesList == nullptr) return;
    auto pCurItem = m_pNotesList->currentItem();
    if (pCurItem) {
        auto pItemWidget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(m_pNoteItem));
        if (pItemWidget) {
            pItemWidget->setBSelect(false);
        }
    }
}

void NotesWidget::addNewItem(const stHighlightContent &note)
{
    if (note.strcontents == QString("")) {
        return;
    }

    NotesItemWidget *itemWidget = new NotesItemWidget;
    itemWidget->setNoteUUid(note.struuid);
    itemWidget->setLabelPage(note.ipage, 1);
    itemWidget->setTextEditText(note.strcontents);
    itemWidget->setMinimumSize(QSize(230, 83));

    QListWidgetItem *item = new QListWidgetItem(m_pNotesList);
    item->setFlags(Qt::NoItemFlags);
    item->setSizeHint(QSize(230, 83));

    m_pNotesList->addItem(item);
    m_pNotesList->setItemWidget(item, itemWidget);
    m_pNoteItem = item;

    m_mapUuidAndPage.insert(note.struuid, note.ipage);
}

/**
 * @brief NotesWidget::addNewItem
 * 增加新的注释item
 * @param image
 * @param page
 * @param uuid
 * @param text
 */
void NotesWidget::addNewItem(const QImage &image, const int &page, const QString &uuid, const QString &text)
{
    auto itemWidget = new NotesItemWidget;
    itemWidget->setLabelImage(image);
    itemWidget->setNoteUUid(uuid);
    itemWidget->setLabelPage(page, 1);
    itemWidget->setTextEditText(text);
    itemWidget->setMinimumSize(QSize(230, 80));

    auto item = new QListWidgetItem(m_pNotesList);
    item->setFlags(Qt::NoItemFlags);
    item->setSizeHint(QSize(230, 80));

    m_pNotesList->addItem(item);
    m_pNotesList->setItemWidget(item, itemWidget);
    m_pNoteItem = item;
}

/**
 * @brief NotesWidget::flushNoteItemText
 * 刷新注释内容
 * @param page
 * @param uuid
 * @param text
 */
void NotesWidget::flushNoteItemText(const int &page, const QString &uuid, const QString &text)
{
    int iCount = m_pNotesList->count();
    for (int iLoop = 0; iLoop < iCount; iLoop++) {
        auto pItem = m_pNotesList->item(iLoop);
        if (pItem) {
            auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
            if (t_widget) {
                if (t_widget->nPageIndex() == page && t_widget->noteUUId() == uuid) {
                    t_widget->setTextEditText(text);
                    break;
                }
            }
        }
    }
}

/**
 * @brief NotesWidget::dealWithData
 * 处理全局信号函数
 * @return
 */
int NotesWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    //  增加注释消息
    if (MSG_NOTE_ADDITEM == msgType) {
        emit sigAddNewNoteItem(msgContent);
        return ConstantMsg::g_effective_res;
    }

    //  删除注释内容
    if (MSG_NOTE_DLTNOTECONTANT == msgType) {
        emit sigDltNoteContant(msgContent);
        return ConstantMsg::g_effective_res;
    }

    // 移除高亮，删除注释内容，删除注释列表item
    if (MSG_NOTE_DLTNOTEITEM == msgType) {
        emit sigDltNoteItem(msgContent);
    } else if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        emit sigOpenFileOk();
    } else if (MSG_CLOSE_FILE == msgType) {
        emit sigCloseFile();
    } else if (MSG_NOTIFY_KEY_MSG == msgType) {
        if (msgContent == KeyStr::g_del) {
            emit sigDelNoteItem();
        } else if (msgContent == KeyStr::g_pgup || msgContent == KeyStr::g_up) {
            emit sigJumpToPrevItem();
        } else if (msgContent == KeyStr::g_pgdown || msgContent == KeyStr::g_down) {
            emit sigJumpToNextItem();
        }
    }

    return 0;
}

/*********************class ThreadLoadImageOfNote***********************/
/**********************加载注释列表***************************************/

ThreadLoadImageOfNote::ThreadLoadImageOfNote(QObject *parent):
    QThread(parent)
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
        int t_page = -1;
        QImage image;
        bool bl = false;

        for (int page = 0; page < m_stListNote.count(); page++) {
            if (!m_isLoaded)
                break;
            auto dproxy = DocummentFileHelper::instance();
            if (nullptr == dproxy) {
                break;
            }

            stHighlightContent highContent = m_stListNote.at(page);

            if (highContent.strcontents == QString("")) {
                continue;
            }

            if (t_page != highContent.ipage) {
                t_page = highContent.ipage;
                bl = dproxy->getImage(t_page, image, 34, 54);
            }
            if (bl) {
                emit sigLoadImage(image);
            }
            msleep(20);
        }

        m_isLoaded = false;
        m_stListNote.clear();
        break;
    }
}
