#include "NotesWidget.h"

#include "translator/PdfControl.h"

#include <DIconButton>

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
void NotesWidget::slotAddNoteItem(QString uuid)
{
    qDebug() << "           NotesWidget::slotAddNoteItem               ";

    int t_page = DocummentProxy::instance()->currentPageNo();
    DocummentProxy *dproxy = DocummentProxy::instance();
    if (nullptr == dproxy) {
        return;
    }
    QImage image;
    dproxy->getImage(t_page, image, 150, 150);

    addNotesItem(image, t_page, "");
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
                    // todo

                    break;
                }
            }
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
void NotesWidget::addNotesItem(const QImage &image, const int &page, const QString &text)
{
    NotesItemWidget *itemWidget = new NotesItemWidget;
    itemWidget->setLabelImage(image);
    itemWidget->setNoteUUid(QString::number(m_nUUid));
    itemWidget->setLabelPage(page, 1);
    itemWidget->setTextEditText(text);
    itemWidget->setMinimumSize(QSize(250, 150));

    QListWidgetItem *item = new QListWidgetItem(m_pNotesList);
    item->setFlags(Qt::NoItemFlags);
    item->setSizeHint(QSize(250, 150));

    m_pNotesList->addItem(item);
    m_pNotesList->setItemWidget(item, itemWidget);

    ++m_nUUid;//测试专用
}

/**
 * @brief NotesWidget::initConnection
 * 初始化信号槽
 */
void NotesWidget::initConnection()
{
    connect(this, SIGNAL(sigDltNoteItem(QString)), this, SLOT(slotDltNoteItem(QString)));

    connect(this, SIGNAL(sigAddNewNoteItem(QString)), this, SLOT(slotAddNoteItem(QString)));
}

/**
 * @brief NotesWidget::dealWithData
 * 处理全局信号函数
 * @return
 */
int NotesWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    //  删除注释消息
    if (MSG_BOOKMARK_DLTITEM == msgType) {
        emit sigDltNoteItem(msgContent);
        return ConstantMsg::g_effective_res;
    }

    //  增加注释消息
    if (MSG_NOTE_ADDITEM == msgType) {
        emit sigAddNewNoteItem(msgContent);
        return ConstantMsg::g_effective_res;
    }

    return 0;
}
