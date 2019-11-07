#include "SearchResWidget.h"
#include "controller/DataManager.h"
#include "frame/DocummentFileHelper.h"

SearchResWidget::SearchResWidget(CustomWidget *parent) :
    CustomWidget(QString("SearchResWidget"), parent)
{
    setObjectName("SearchResWidget");
    m_loadSearchResThread.setSearchResW(this);

    initWidget();

    initConnections();
}

SearchResWidget::~SearchResWidget()
{
    if (m_loadSearchResThread.isRunning()) {
        m_loadSearchResThread.stopThread();
    }
}

void SearchResWidget::slotClearWidget()
{
    DocummentFileHelper::instance()->clearsearch();
    if (m_pNotesList) {
        m_pNotesList->clear();
    }
}

void SearchResWidget::slotCloseFile()
{
    if (m_loadSearchResThread.isRunning()) {
        m_loadSearchResThread.stopThread();
    }

    slotClearWidget();
}

void SearchResWidget::slotFlushSearchWidget(const QString &msgContent)
{
    connect(DocummentProxy::instance(), SIGNAL(signal_searchRes(stSearchRes)), this, SLOT(slotGetSearchContant(stSearchRes)));
    QMap<int, stSearchRes> resMap;
    DocummentFileHelper::instance()->search(msgContent, resMap, Qt::red);
    disconnect(DocummentProxy::instance(), SIGNAL(signal_searchover()), this, SLOT(slotSearchOver()));
    connect(DocummentProxy::instance(), SIGNAL(signal_searchover()), this, SLOT(slotSearchOver()));
}

void SearchResWidget::slotGetSearchContant(stSearchRes search)
{
    if (search.listtext.size() < 1) {
        return;
    }
    m_loadSearchResThread.pushSearch(search);
}

void SearchResWidget::slotSearchOver()
{
    if (m_loadSearchResThread.isRunning()) {
        m_loadSearchResThread.stopThread();
    }

    disconnect(DocummentProxy::instance(), SIGNAL(signal_searchRes(stSearchRes)), this, SLOT(slotGetSearchContant(stSearchRes)));

    //生成左侧搜索列表
    //to do and send flush thumbnail and contant
    initSearchList(m_loadSearchResThread.searchList());
}

void SearchResWidget::slotLoadImage(const int &page, const QImage &image)
{
    if (m_pSearchItemWidget) {
        if (page == m_pSearchItemWidget->nPageIndex()) {
            m_pSearchItemWidget->setLabelImage(image);
        }
    }
}

void SearchResWidget::slotFindPrev()
{
    DocummentFileHelper::instance()->findPrev();
}

void SearchResWidget::slotFindNext()
{
    DocummentFileHelper::instance()->findNext();
}

void SearchResWidget::initWidget()
{
    auto m_pVLayout  = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    m_pNotesList = new CustomListWidget;

    m_pVLayout->addWidget(m_pNotesList);
}

void SearchResWidget::initConnections()
{
    connect(&m_loadSearchResThread, SIGNAL(sigLoadImage(const int &, const QImage &)),
            this, SLOT(slotLoadImage(const int &, const QImage &)));

    connect(this, SIGNAL(sigClearWidget()), this, SLOT(slotClearWidget()));
    connect(this, SIGNAL(sigFlushSearchWidget(const QString &)), this, SLOT(slotFlushSearchWidget(const QString &)));
    connect(this, SIGNAL(sigCloseFile()), this, SLOT(slotCloseFile()));
    connect(this, SIGNAL(sigFindPrev()), this, SLOT(slotFindPrev()));
    connect(this, SIGNAL(sigFindNext()), this, SLOT(slotFindNext()));
}

void SearchResWidget::initSearchList(const QList<stSearchRes> &list)
{
    //  插入之前 先清空原有数据
    slotClearWidget();

    int resultNum = 0;
    QString strText = "";

    foreach (stSearchRes it, list) {
        int page = it.ipage;
        foreach (QString s, it.listtext) {
            strText += s.trimmed();
            strText += QString("    ");

            ++resultNum;
        }

        addSearchsItem(page, strText, resultNum);
        resultNum = 0;
        strText.clear();
    }

    //开始填充缩略图线程
    m_loadSearchResThread.setRunning(true);
    m_loadSearchResThread.start();

    //  当前显示不是自己, 则需要发送切换 ListWidget 显示
//    bool bFocus = this->hasFocus();
//    if (bFocus) {
    sendMsg(MSG_SWITCHLEFTWIDGET, QString("3"));
//    }
}

void SearchResWidget::addSearchsItem(const int &page, const QString &text, const int &resultNum)
{
    auto itemWidget = new NotesItemWidget;
    itemWidget->setNoteSigne(false);
    itemWidget->setLabelPage(page, 1);
    itemWidget->setTextEditText(text);
    itemWidget->setSerchResultText((QString("   %1").arg(resultNum) + tr("search res content")));
    itemWidget->setMinimumSize(QSize(240, 80));

    auto item = new QListWidgetItem(m_pNotesList);
    item->setFlags(Qt::NoItemFlags);
    item->setSizeHint(QSize(240, 80));

    m_pNotesList->addItem(item);
    m_pNotesList->setItemWidget(item, itemWidget);
}

/**
 * @brief SearchResWidget::dealWithData
 * 处理全局信号函数
 * @param msgType
 * @param msgContent
 * @return
 */
int SearchResWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_FIND_PREV) {
        emit sigFindPrev();
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_FIND_NEXT) {
        emit sigFindNext();
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_FIND_CONTENT) {        //  查询内容
        if (msgContent != QString("")) {
            emit sigFlushSearchWidget(msgContent);
        }
    } else if (msgType == MSG_CLEAR_FIND_CONTENT) {
        emit sigClearWidget();
    } else if (MSG_CLOSE_FILE == msgType) {    //  关闭w文件通知消息
        emit sigCloseFile();
    }

    return 0;
}

/**
 * @brief SearchResWidget::getSearchPage
 * 得到当前搜索页码
 * @param index
 * @return
 */
int SearchResWidget::getSearchPage(const int &index)
{
    QListWidgetItem *pItem = m_pNotesList->item(index);
    m_pSearchItemWidget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
    if (m_pSearchItemWidget) {
        return m_pSearchItemWidget->nPageIndex();
    }

    return -1;
}

/************************************LoadSearchResList*******************************************************/
/************************************加载搜索列表缩略图*********************************************************/

LoadSearchResThread::LoadSearchResThread(QObject *parent):
    QThread(parent)
{

}

/**
 * @brief LoadSearchResThread::stopThread
 * 停止线程
 */
void LoadSearchResThread::stopThread()
{
    m_isRunning = false;

    quit();
//    terminate();    //终止线程
    wait();         //阻塞等待
}

/**
 * @brief LoadSearchResThread::run
 * 线程主工作区
 */
void LoadSearchResThread::run()
{
    m_pages = m_searchContantList.count();
    m_searchContantList.clear();

    m_nStartIndex = 0;
    m_nEndIndex = 19;

    while (m_isRunning) {

        if (m_nStartIndex < 0) {
            m_nStartIndex = 0;
        }
        if (m_nEndIndex >= m_pages) {
            m_nEndIndex = m_pages - 1;
        }

        int page = -1;

        for (int index = m_nStartIndex; index <= m_nEndIndex; index++) {

            if (!m_isRunning) {
                break;
            }

            if (!m_pSearchResWidget) {
                break;
            }

            page = m_pSearchResWidget->getSearchPage(index);

            if (page == -1) {
                continue;
            }
            auto dproxy = DocummentFileHelper::instance();
            if (nullptr == dproxy) {
                break;
            }


            QImage image;
            bool bl = dproxy ->getImage(page, image, 28, 50);
            if (bl) {
                emit sigLoadImage(page, image);
                msleep(10);
            }
        }

        m_nStartIndex += FIRST_LOAD_PAGES;
        m_nEndIndex   += FIRST_LOAD_PAGES;

        msleep(30);
    }
}
