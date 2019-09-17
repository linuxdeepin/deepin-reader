#include "BookMarkWidget.h"
#include <QDebug>

BookMarkWidget::BookMarkWidget(CustomWidget *parent) :
    CustomWidget("BookMarkWidget", parent)
{
    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(0);
    this->setLayout(m_pVBoxLayout);

    initWidget();

    initConnection();
}
/**
 * @brief BookMarkWidget::slotShowSelectItem
 * @param 点击左侧书签列表, fileView跳转相应的页
 */
void BookMarkWidget::slotShowSelectItem(QListWidgetItem *item)
{
    BookMarkItemWidget *t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
    if (t_widget) {
        int nnPageNumber = t_widget->PageNumber();
        //m_nCurrentPage = nnPageNumber;

        DocummentProxy::instance()->pageJump(nnPageNumber);
    }
}

/**
 * @brief BookMarkWidget::slotAddBookMark
 * 增加书签item
 */
void BookMarkWidget::slotAddBookMark()
{
    int page = m_nCurrentPage;

    if (m_pAllPageList.contains(page)) {
        return;
    }

    if (m_pAllPageList.size() == 0) {
        //bool rl =
        dApp->dbM->insertBookMark(QString::number(page));

    } else {
        QString sPage = "";
        foreach (int i, m_pAllPageList) {
            sPage += QString::number(i) + tr(",");
        }

        sPage += QString::number(page);
        dApp->dbM->updateBookMark(QString::number(page));
    }

    m_pAllPageList.append(page);
    addBookMarkItem(page);

    slotDocFilePageChanged(page);
}

/**
 * @brief BookMarkWidget::slotOpenFileOk
 *  打开文件成功，　获取该文件的书签数据
 */

void BookMarkWidget::slotOpenFileOk()
{
    connect(DocummentProxy::instance(), SIGNAL(signal_pageChange(int)), this, SLOT(slotDocFilePageChanged(int)), Qt::QueuedConnection);

    QString sAllPages = dApp->dbM->getBookMarks();
    QStringList sPageList = sAllPages.split(",", QString::SkipEmptyParts);
    foreach (QString s, sPageList) {
        int nPage = s.toInt();

        m_pAllPageList.append(nPage);

        addBookMarkItem(nPage);
    }

    slotDocFilePageChanged(0);
}

/**
 * @brief BookMarkWidget::slotDocFilePageChanged
 * @param page:当前活动页，页码
 */
void BookMarkWidget::slotDocFilePageChanged(int page)
{
    m_nCurrentPage = page;

    bool bl =  m_pAllPageList.contains(page);

    m_pAddBookMarkBtn->setEnabled(!bl);
    sendMsg(MSG_BOOKMARK_STATE, QString::number(bl));
}

/**
 * @brief BookMarkWidget::slotDeleteBookItem
 * 按页码删除书签
 * @param nPage：要删除的书签页
 */
void BookMarkWidget::slotDeleteBookItem(const int &nPage)
{
    int nCount = m_pBookMarkListWidget->count();

    for (int iLoop = 0; iLoop < nCount; iLoop++) {
        QListWidgetItem *pItem = m_pBookMarkListWidget->item(iLoop);
        BookMarkItemWidget *t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
        if (t_widget) {
            int nPageNumber = t_widget->PageNumber();
            if (nPageNumber == nPage) {
                m_pAllPageList.removeOne(nPage);

                t_widget->deleteLater();
                t_widget = nullptr;

                delete  pItem;

                slotDocFilePageChanged(nPage);
                break;
            }
        }
    }
}

/**
 * @brief BookMarkWidget::initWidget
 * 初始化书签窗体
 */
void BookMarkWidget::initWidget()
{
    m_pBookMarkListWidget = new DListWidget;
    m_pBookMarkListWidget->setSpacing(10);
    //设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
    m_pBookMarkListWidget->setResizeMode(QListWidget::Adjust);

    m_pAddBookMarkBtn = new DImageButton;
    m_pAddBookMarkBtn->setFixedSize(QSize(250, 50));
    m_pAddBookMarkBtn->setText(tr("adding bookmark"));
    connect(m_pAddBookMarkBtn, SIGNAL(clicked()), this, SLOT(slotAddBookMark()));

    connect(this, SIGNAL(sigAddBookMark()), this, SLOT(slotAddBookMark()));

    m_pVBoxLayout->addWidget(m_pBookMarkListWidget);
    m_pVBoxLayout->addWidget(m_pAddBookMarkBtn);
}

/**
 * @brief BookMarkWidget::keyPressEvent
 * 响应键盘事件
 * @param e
 */
void BookMarkWidget::keyPressEvent(QKeyEvent *e)
{
    QString key = Utils::getKeyshortcut(e);

    if (key == "Del") {
        dltItem();
    }  else {
        // Pass event to CustomWidget continue, otherwise you can't type anything after here. ;)
        CustomWidget::keyPressEvent(e);
    }
}

/**
 * @brief BookMarkWidget::initConnection
 *初始化connect
 */
void BookMarkWidget::initConnection()
{
    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigDeleteBookItem(const int &)), this, SLOT(slotDeleteBookItem(const int &)));
    connect(m_pBookMarkListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotShowSelectItem(QListWidgetItem *)));
}

/**
 * @brief BookMarkWidget::dltItem
 * 按键删除书签页
 */
void BookMarkWidget::dltItem()
{
    slotDeleteBookItem(m_nCurrentPage);
//    m_nCurrentPage = -1;
//    QList<QListWidgetItem *> items =  m_pBookMarkListWidget->selectedItems();
//    foreach (QListWidgetItem *item, items) {

//        BookMarkItemWidget *t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
//        if (t_widget) {
//            int nnPageNumber = t_widget->PageNumber();

//            m_pAllPageList.removeOne(nnPageNumber);

//            t_widget->deleteLater();
//            t_widget = nullptr;
//        }
//        delete  item;
//    }
}

/**
 * @brief BookMarkWidget::addBookMarkItem
 * 添加书签
 * @param page：要增加的书签页
 */
void BookMarkWidget::addBookMarkItem(const int &page)
{
    QImage image;

    DocummentProxy::instance()->getImage(page, image, 130, 150);

    BookMarkItemWidget *t_widget = new BookMarkItemWidget(this);
    t_widget->setItemImage(image);
    t_widget->setPageNumber(page);
    t_widget->setMinimumSize(QSize(250, 150));

    QListWidgetItem *item = new QListWidgetItem(m_pBookMarkListWidget);
    item->setFlags(Qt::ItemIsSelectable);
    item->setSizeHint(QSize(250, 150));

    m_pBookMarkListWidget->addItem(item);
    m_pBookMarkListWidget->setItemWidget(item, t_widget);
}

/**
 * @brief BookMarkWidget::dealWithData
 * 处理全局消息
 * @param msgType:消息类型
 * @param msgContent:消息内容
 * @return
 */
int BookMarkWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_BOOKMARK_DLTITEM == msgType) {

        int nPage = msgContent.toInt();

        emit sigDeleteBookItem(nPage);
        qDebug() << "dlt bookmark item by menu";
        return ConstantMsg::g_effective_res;
    }

    if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        emit sigOpenFileOk();
    }

    if (MSG_BOOKMARK_ADDITEM == msgType) {
        emit sigAddBookMark();
    }

    return 0;
}
