#include "BookMarkWidget.h"
#include <QDebug>
#include "translator/PdfControl.h"

BookMarkWidget::BookMarkWidget(CustomWidget *parent) :
    CustomWidget(QString("BookMarkWidget"), parent)
{
    m_loadBookMarkThread.setBookMark(this);

    initWidget();

    initConnection();
}

BookMarkWidget::~BookMarkWidget()
{
    if (m_loadBookMarkThread.isRunning()) {
        m_loadBookMarkThread.stopThreadRun();
    }
}

/**
 * @brief BookMarkWidget::slotAddBookMark
 * 增加书签item
 */
void BookMarkWidget::slotAddBookMark()
{

    if (m_pAllPageList.contains(m_nCurrentPage)) {
        return;
    }

    int page = m_nCurrentPage;

    if (m_pAllPageList.size() == 0) {
        //bool rl =
        dApp->dbM->insertBookMark(QString::number(page));

    } else {
        QString sPage = "";
        foreach (int i, m_pAllPageList) {
            sPage += QString::number(i) + ",";
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
    disconnect(DocummentProxy::instance(), SIGNAL(signal_pageChange(int)), this, SLOT(slotDocFilePageChanged(int)));
    connect(DocummentProxy::instance(), SIGNAL(signal_pageChange(int)), this, SLOT(slotDocFilePageChanged(int)), Qt::QueuedConnection);

    m_pAllPageList.clear();

    QString sAllPages = dApp->dbM->getBookMarks();
    QStringList sPageList = sAllPages.split(",", QString::SkipEmptyParts);
    foreach (QString s, sPageList) {
        int nPage = s.toInt();

        m_pAllPageList.append(nPage);

        addBookMarkItem(nPage);
    }

    m_loadBookMarkThread.setBookMarks(m_pAllPageList.count());
    m_loadBookMarkThread.start();

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
void BookMarkWidget::slotDeleteBookItem()
{
    int nCount = m_pBookMarkListWidget->count();

    for (int iLoop = 0; iLoop < nCount; iLoop++) {
        QListWidgetItem *pItem = m_pBookMarkListWidget->item(iLoop);
        BookMarkItemWidget *t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
        if (t_widget) {
            int nPageNumber = t_widget->nPageIndex();
            if (nPageNumber == m_nCurrentPage) {
                m_pAllPageList.removeOne(m_nCurrentPage);

                t_widget->deleteLater();
                t_widget = nullptr;

                delete  pItem;

                slotDocFilePageChanged(m_nCurrentPage);
                break;
            }
        }
    }
}

void BookMarkWidget::slotCloseFile()
{
    if (m_loadBookMarkThread.isRunning()) {
        m_loadBookMarkThread.stopThreadRun();
    }
}

void BookMarkWidget::slot_loadImage(int page, QImage image)
{
    m_pBookMarkListWidget->setItemImage(page, image);
}

/**
 * @brief BookMarkWidget::initWidget
 * 初始化书签窗体
 */
void BookMarkWidget::initWidget()
{
    m_pBookMarkListWidget = new CustomListWidget;

    m_pAddBookMarkBtn = new DPushButton(this);
    m_pAddBookMarkBtn->setMinimumSize(QSize(250, 50));
    m_pAddBookMarkBtn->setText(PdfControl::ADD_BK);
    connect(m_pAddBookMarkBtn, SIGNAL(clicked()), this, SLOT(slotAddBookMark()));

    connect(this, SIGNAL(sigAddBookMark()), this, SLOT(slotAddBookMark()));

    auto m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(0);
    this->setLayout(m_pVBoxLayout);
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

    if (key == PdfControl::DEL_KEY) {
        slotDeleteBookItem();
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
    connect(&m_loadBookMarkThread, SIGNAL(signal_loadImage(int, QImage)), this, SLOT(slot_loadImage(int, QImage)));
    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigDeleteBookItem()), this, SLOT(slotDeleteBookItem()));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));
}

/**
 * @brief BookMarkWidget::addBookMarkItem
 * 添加书签
 * @param page：要增加的书签页
 */
void BookMarkWidget::addBookMarkItem(const int &page)
{
    QImage t_image;

    bool rl = DocummentProxy::instance()->getImage(page, t_image, 113, 143);
    if (rl) {
        BookMarkItemWidget *t_widget = new BookMarkItemWidget;
        t_widget->setLabelImage(t_image);
        t_widget->setLabelPage(page, 1);
        t_widget->setMinimumSize(QSize(250, 150));

        QListWidgetItem *item = new QListWidgetItem(m_pBookMarkListWidget);
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(250, 150));

        m_pBookMarkListWidget->addItem(item);
        m_pBookMarkListWidget->setItemWidget(item, t_widget);
    }
}

/**
 * @brief BookMarkWidget::dealWithData
 * 处理全局消息
 * @param msgType:消息类型
 * @param msgContent:消息内容
 * @return
 */
int BookMarkWidget::dealWithData(const int &msgType, const QString &)
{
    //  删除书签消息
    if (MSG_BOOKMARK_DLTITEM == msgType) {
        emit sigDeleteBookItem();
        return ConstantMsg::g_effective_res;
    }

    //  增加书签消息
    if (MSG_BOOKMARK_ADDITEM == msgType || MSG_OPERATION_ADD_BOOKMARK == msgType) {
        emit sigAddBookMark();
        return ConstantMsg::g_effective_res;
    }

    //  打开w文件通知消息
    if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        emit sigOpenFileOk();
    }

    //  关闭w文件通知消息
    if (MSG_CLOSE_FILE == msgType) {
        emit sigCloseFile();
    }

    return 0;
}

int BookMarkWidget::getBookMarkPage(const int &index)
{
    QListWidgetItem *pItem = m_pBookMarkListWidget->item(index);
    m_pItemWidget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
    if (m_pItemWidget) {
        int page = m_pItemWidget->nPageIndex();
        if (m_pAllPageList.contains(page)) {
            return page;
        }
    }

    return -1;
}

/*************************************LoadBookMarkThread**************************************/
/*************************************加载书签列表线程******************************************/

LoadBookMarkThread::LoadBookMarkThread(QObject *parent)
    : QThread (parent)
{

}

void LoadBookMarkThread::stopThreadRun()
{
    m_isRunning = false;

    terminate();    //终止线程
    wait();         //阻塞等待
}

void LoadBookMarkThread::run()
{
    while (m_isRunning) {

        if (m_nStartIndex < 0) {
            m_nStartIndex = 0;
        }
        if (m_nEndIndex >= m_bookMarks) {
            m_isRunning = false;
            m_nEndIndex = m_bookMarks - 1;
        }

        for (int index = m_nStartIndex; index <= m_nEndIndex; index++) {
            QImage image;
            int page = -1;

            if (!m_isRunning)
                break;

            if (!m_pBookMarkWidget) {
                continue;
            }

            page = m_pBookMarkWidget->getBookMarkPage(index);

            if (page == -1) {
                continue;
            }

            bool bl = DocummentProxy::instance()->getImage(page, image, 113, 143);
            if (bl) {
                emit signal_loadImage(page, image);
            }
        }

        m_nStartIndex += FIRST_LOAD_PAGES;
        m_nEndIndex += FIRST_LOAD_PAGES;

        msleep(50);
    }
}
