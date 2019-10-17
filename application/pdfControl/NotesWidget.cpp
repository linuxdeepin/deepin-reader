#include "NotesWidget.h"
#include "controller/DataManager.h"

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
    for (int row = 0; row < m_pNotesList->count(); ++row) {
        QListWidgetItem *pItem = m_pNotesList->item(row);
        if (pItem) {
            NotesItemWidget *t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
            if (t_widget) {
                QString t_uuid = t_widget->noteUUId();
                if (t_uuid == uuid) {
                    delete t_widget;
                    t_widget = nullptr;

                    delete  pItem;
                    pItem = nullptr;

                    // remove date from map and notify kong yun zhen
                    removeFromMap(uuid);

                    auto dproxy = DocummentProxy::instance();

                    if(dproxy){
                        DataManager::instance()->setBIsUpdate(true);
                        dproxy->removeAnnotation(uuid);
                        qDebug() << "NotesWidget::slotDltNoteItem uuid:" << uuid;
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
    foreach(auto note, m_mapNotes)
    {
        if (note.contains(uuid)) {
            note.remove(uuid);
            return;
        }
    }
}

void NotesWidget::slotOpenFileOk()
{
    m_ThreadLoadImage.setIsLoaded(false);
    if (m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }

    auto t_docPtr = DocummentProxy::instance();
    if(!t_docPtr){
        return;
    }

    QList<stHighlightContent> list_note;
    list_note.clear();

    t_docPtr->getAllAnnotation(list_note);

    if(list_note.count() < 1){
        return;
    }

    m_mapNotes.clear();
    foreach(auto st, list_note)
    {
        addNewItem();

        addNoteToMap(st);
    }

    m_ThreadLoadImage.setListNoteSt(list_note);
    m_ThreadLoadImage.setIsLoaded(true);
    if (!m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.start();
    }
}

void NotesWidget::slotCloseFile()
{
    m_ThreadLoadImage.setIsLoaded(false);
    if (!m_ThreadLoadImage.isRunning()) {
        m_ThreadLoadImage.stopThreadRun();
    }

    m_pNotesList->clear();
}

void NotesWidget::slotLoadImage(const int &page, const QImage &image, const QString &uuid, const QString &contant)
{
    static int index = 0;

    if(m_pNotesList->count() < 1 || index >= m_pNotesList->count()){
        return;
    }

    QListWidgetItem *pItem = m_pNotesList->item(index);
    if (pItem) {
        NotesItemWidget *t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
        if (t_widget) {
            t_widget->setNoteUUid(uuid);
            t_widget->setTextEditText(contant);
            t_widget->setLabelPage(page,1);
            t_widget->setLabelImage(image);
        }
    }
    ++index;
}

void NotesWidget::slotDelNoteItem()
{
    if(!m_pNoteItem){
        return;
    }

    NotesItemWidget *t_widget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(m_pNoteItem));
    if (t_widget) {
        QString t_uuid = t_widget->noteUUId();

        delete t_widget;
        t_widget = nullptr;

        delete  m_pNoteItem;
        m_pNoteItem = nullptr;

        // remove date from map and notify kong yun zhen
        removeFromMap(t_uuid);

        auto t_pDocummentProxy = DocummentProxy::instance();
        if(t_pDocummentProxy){
            t_pDocummentProxy->removeAnnotation(t_uuid);
        }
    }
}

void NotesWidget::slotSelectItem(QListWidgetItem *item)
{
    m_pNoteItem = item;
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
        t_strUUid = t_strList.at(0).trimmed();
        t_strText = t_strList.at(1).trimmed();
        t_nPage = t_strList.at(2).trimmed().toInt();
    }else {
        return;
    }

    bool b_has = hasNoteInList(t_nPage, t_strUUid);

    if(b_has){
        flushNoteItemText(t_nPage, t_strUUid, t_strText);
    }else{
        DocummentProxy *dproxy = DocummentProxy::instance();
        if (nullptr == dproxy) {
            return;
        }
        dproxy->getImage(t_nPage, image, 113, 143);
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

    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));
    connect(&m_ThreadLoadImage, SIGNAL(sigLoadImage(const int &, const QImage &, const QString&, const QString&)),
            this, SLOT(slotLoadImage(const int &, const QImage &, const QString&, const QString&)));
    connect(this, SIGNAL(sigDelNoteItem()), this, SLOT(slotDelNoteItem()));
    connect(m_pNotesList, SIGNAL(sigSelectItem(QListWidgetItem*)), this, SLOT(slotSelectItem(QListWidgetItem*)));
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

void NotesWidget::addNewItem()
{
    NotesItemWidget *itemWidget = new NotesItemWidget;
    itemWidget->setNoteUUid(QString(""));
    itemWidget->setLabelPage(-1, 1);
    itemWidget->setTextEditText(QString(""));
    itemWidget->setMinimumSize(QSize(250, 150));

    QListWidgetItem *item = new QListWidgetItem(m_pNotesList);
    item->setFlags(Qt::NoItemFlags);
    item->setSizeHint(QSize(250, 150));

    m_pNotesList->addItem(item);
    m_pNotesList->setItemWidget(item, itemWidget);
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

void NotesWidget::addNoteToMap(const stHighlightContent & note)
{
    int t_page = note.ipage;
    QString t_strUUid = note.struuid;
    QString t_strContant = note.strcontents;
    QMap<QString, QString> t_node;

    if(m_mapNotes.contains(t_page)){
        t_node = m_mapNotes.value(t_page);
        m_mapNotes.remove(t_page);
    }

    t_node.insert(t_strUUid, t_strContant);
    m_mapNotes.insert(t_page, t_node);
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
        return ConstantMsg::g_effective_res;
    }

    if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        emit sigOpenFileOk();
    } else if (MSG_CLOSE_FILE == msgType) {
        emit sigCloseFile();
    }

    if(MSG_NOTIFY_KEY_MSG == msgType){
        if (msgContent == QString("Del")) {
            emit sigDelNoteItem();
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
        QString t_strUUid(""),t_noteContant("");
        QImage image;
        bool bl = false;

        for (int page = 0; page < m_stListNote.count(); page++) {
            if (!m_isLoaded)
                break;
            DocummentProxy *dproxy = DocummentProxy::instance();
            if (nullptr == dproxy) {
                break;
            }

            if(t_page != m_stListNote.at(page).ipage){
                t_page = m_stListNote.at(page).ipage;
                bl = dproxy->getImage(t_page, image, 113, 143);
            }

            t_strUUid = m_stListNote.at(page).struuid;
            t_noteContant = m_stListNote.at(page).strcontents;

            if (bl) {
                emit sigLoadImage(t_page, image, t_strUUid, t_noteContant);
            }

            msleep(20);
        }

        m_isLoaded = false;
        m_stListNote.clear();
        break;
    }
}
