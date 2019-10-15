#include "NotesWidget.h"

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

    m_pVLayout->addWidget(m_pNotesList);
}

void NotesWidget::keyPressEvent(QKeyEvent *e)
{
    QString key = Utils::getKeyshortcut(e);

    if (key == PdfControl::DEL_KEY) {
        QListWidgetItem *pItem = m_pNotesList->currentItem();
        if (pItem) {
            NotesItemWidget *t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
            if (t_widget) {
                QString t_strUUid = t_widget->noteUUId();

                t_widget->deleteLater();
                t_widget = nullptr;

                delete  pItem;
                pItem = nullptr;

                sendMsg(MSG_BOOKMARK_DLTITEM, t_strUUid);
            }
        }
    }  else {
        // Pass event to CustomWidget continue, otherwise you can't type anything after here. ;)
        CustomWidget::keyPressEvent(e);
    }
}

/**
 * @brief NotesWidget::slotAddNoteItem
 * 增加注释缩略图Item槽函数
 */
void NotesWidget::slotAddNoteItem(QString note)
{
    addNotesItem(note);
}

/**
 * @brief NotesWidget::slotDltNoteItem
 * 右键删除当前注释item
 */
void NotesWidget::slotDltNoteItem(QString uuid)
{
    for (int row = 0; m_pNotesList->count(); ++row) {
        QListWidgetItem *pItem = m_pNotesList->item(row);
        if (pItem) {
            NotesItemWidget *t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
            if (t_widget) {
                QString t_uuid = t_widget->noteUUId();
                if (t_uuid == uuid) {
                    t_widget->deleteLater();
                    t_widget = nullptr;

                    delete  pItem;

                    // remove date from map and notify kong yun zhen
                    removeFromMap(uuid);

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
    foreach(auto note, m_mapNotes)
    {
        if (note.contains(uuid)) {
            note.remove(uuid);
            note.insert(uuid, QString(""));
            return;
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
    QString t_strUUid,t_strText;
    int t_nPage = -1;
    QImage image;

    QStringList t_strList = text.split(QString("%"));

    if(t_strList.count() == 3){
        DocummentProxy *dproxy = DocummentProxy::instance();
        if (nullptr == dproxy) {
            return;
        }

        t_strUUid = t_strList.at(0).trimmed();
        t_strText = t_strList.at(1).trimmed();
        t_nPage = t_strList.at(2).toInt();

        dproxy->getImage(t_nPage, image, 113, 143);
    }

    bool b_has = hasNoteInList(t_nPage, t_strUUid);

    if(b_has){
        flushNoteItemText(t_nPage, t_strUUid, t_strText);
    }else{
        addNewItem(image, t_nPage, t_strUUid, t_strText);

        QMap<QString, QString> t_contant;
        t_contant.clear();
        t_contant.insert(t_strUUid, t_strText);
        m_mapNotes.insert(t_nPage, t_contant);
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

    connect(this, SIGNAL(sigAddNewNoteItem(QString)), this, SLOT(slotAddNoteItem(QString)));
}

/**
 * @brief NotesWidget::hasNoteInList
 * 判断本地是否有该条注释
 * @param page
 * @param uuid
 * @return
 */
bool NotesWidget::hasNoteInList(const int &page, const QString &uuid)
{
    if(m_mapNotes.contains(page)){
        foreach(auto contant, m_mapNotes){
            if(contant.contains(uuid)){
                return true;
            }
        }
    }

    return false;
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
    NotesItemWidget *itemWidget = new NotesItemWidget;
    itemWidget->setLabelImage(image);
    itemWidget->setNoteUUid(uuid);
    itemWidget->setLabelPage(page, 1);
    itemWidget->setTextEditText(text);
    itemWidget->setMinimumSize(QSize(250, 150));

    QListWidgetItem *item = new QListWidgetItem(m_pNotesList);
    item->setFlags(Qt::NoItemFlags);
    item->setSizeHint(QSize(250, 150));

    m_pNotesList->addItem(item);
    m_pNotesList->setItemWidget(item, itemWidget);
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
    if(m_pNotesList->count() < 1){
        return;
    }

    for(int index = 0; index < m_pNotesList->count(); ++index){
        QListWidgetItem *pItem = m_pNotesList->item(index);
        if (pItem) {
            NotesItemWidget *t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
            if (t_widget) {
                if(t_widget->nPageIndex() == page && t_widget->noteUUId() == uuid){
                    t_widget->setTextEditText(text);
                }
            }
        }
    }
}

void NotesWidget::removeFromMap(const QString &uuid) const
{
    foreach(auto node, m_mapNotes){
        if(node.contains(uuid)){
            node.remove(uuid);
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

    //  删除注释内容消息
    if (MSG_NOTE_DLTNOTECONTANT == msgType) {
        emit sigDltNoteContant(msgContent);
        return ConstantMsg::g_effective_res;
    }

    // 移除高亮，删除注释内容，删除注释列表item
    if (MSG_NOTE_DLTNOTEITEM == msgType) {
        emit sigDltNoteItem(msgContent);
        return ConstantMsg::g_effective_res;
    }

    return 0;
}
