#include "SearchResWidget.h"
#include "controller/DataManager.h"
#include "frame/DocummentFileHelper.h"

SearchResWidget::SearchResWidget(CustomWidget *parent) :
    CustomWidget(QString("SearchResWidget"), parent)
{
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
    if (m_pSearchList->count() > 0) {
        DocummentFileHelper::instance()->clearsearch();

        m_pSearchList->clear();
    }

    bool bShowThunmb = DataManager::instance()->bThumbnIsShow();

    if (!bShowThunmb) {
        //  侧边栏 隐藏
        notifyMsg(MSG_SLIDER_SHOW_STATE, "0");
    }
    notifyMsg(MSG_SWITCHLEFTWIDGET, QString::number(DataManager::instance()->currentWidget()));
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

    if (m_pSearchList == nullptr) return;

    m_pSearchList->clear();

    disconnect(DocummentProxy::instance(), SIGNAL(signal_searchRes(stSearchRes)), this, SLOT(slotGetSearchContant(stSearchRes)));

    QList<stSearchRes> list = m_loadSearchResThread.searchList();

    if (list.size() <= 0) {
        showTips();
    } else {
        //生成左侧搜索列表
        //to do and send flush thumbnail and contant
        initSearchList(list);
    }

    notifyMsg(MSG_SWITCHLEFTWIDGET, QString("3"));

    bool t_bTnumbnIsShow = DataManager::instance()->bThumbnIsShow();
    if (!t_bTnumbnIsShow) {
        notifyMsg(MSG_SLIDER_SHOW_STATE, QString::number(!t_bTnumbnIsShow));
    }
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

void SearchResWidget::slotSelectItem(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    setSelectItemBackColor(item);
}

void SearchResWidget::initWidget()
{
    auto m_pVLayout  = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 8, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    m_pSearchList = new CustomListWidget;
    m_pSearchList->setSpacing(1);

    m_pVLayout->addWidget(m_pSearchList);
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
    connect(m_pSearchList, SIGNAL(sigSelectItem(QListWidgetItem *)), this, SLOT(slotSelectItem(QListWidgetItem *)));
}

void SearchResWidget::initSearchList(const QList<stSearchRes> &list)
{
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

//    if(m_pSearchList && m_pSearchList->count() > 0){
//        auto pItem = m_pSearchList->item(0);
//        auto pWidget = reinterpret_cast<NotesItemWidget *>(m_pSearchList->itemWidget(pItem));
//        if(pWidget){
//            pWidget->setBSelect(true);
//            m_pSearchItem = pItem;
//        }
//    }

    //开始填充缩略图线程
    m_loadSearchResThread.setRunning(true);
    m_loadSearchResThread.start();
}

void SearchResWidget::addSearchsItem(const int &page, const QString &text, const int &resultNum)
{
    auto itemWidget = new NotesItemWidget;
    itemWidget->setNoteSigne(false);
    itemWidget->setLabelPage(page, 1);
    itemWidget->setTextEditText(text);
    itemWidget->setSerchResultText((QString("   %1").arg(resultNum) + tr(" results")));
    itemWidget->setMinimumSize(QSize(230, 80));

    auto item = new QListWidgetItem(m_pSearchList);
    item->setFlags(Qt::NoItemFlags);
    item->setSizeHint(QSize(230, 80));

    m_pSearchList->addItem(item);
    m_pSearchList->setItemWidget(item, itemWidget);
}

void SearchResWidget::showTips()
{
    if (m_pSearchList == nullptr) {
        return;
    }

    auto hLayout = new QHBoxLayout;
    auto vLayout = new QVBoxLayout;
    auto tipWidget = new DWidget;
    auto tipLab = new DLabel(tr("no result"));
    tipLab->setForegroundRole(/*DPalette::TextTips*/QPalette::ToolTipText);
    DFontSizeManager::instance()->bind(tipLab, DFontSizeManager::T6);

    qDebug() << this->size();

    tipWidget->setMinimumSize(QSize(226, 528));

    hLayout->addStretch(1);
    hLayout->addWidget(tipLab);
    hLayout->addStretch(1);
    vLayout->addStretch(1);
    vLayout->addItem(hLayout);
    vLayout->addStretch(1);
    tipWidget->setLayout(vLayout);

    auto item = new QListWidgetItem;
    item->setSizeHint(QSize(226, 528));
    m_pSearchList->addItem(item);
    m_pSearchList->setItemWidget(item, tipWidget);
}

void SearchResWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }

    clearItemColor();

    m_pSearchList->setCurrentItem(item);

    auto t_widget = reinterpret_cast<NotesItemWidget *>(m_pSearchList->itemWidget(item));
    if (t_widget) {
        t_widget->setBSelect(true);
        m_pSearchItem = item;
    }
}

void SearchResWidget::clearItemColor()
{
    if (m_pSearchList == nullptr) return;
    auto pCurItem = m_pSearchList->currentItem();
    if (pCurItem) {
        auto pItemWidget = reinterpret_cast<NotesItemWidget *>(m_pSearchList->itemWidget(m_pSearchItem));
        if (pItemWidget) {
            pItemWidget->setBSelect(false);
        }
    }
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
    auto pItem = m_pSearchList->item(index);
    if (pItem) {
        m_pSearchItemWidget = reinterpret_cast<NotesItemWidget *>(m_pSearchList->itemWidget(pItem));
        if (m_pSearchItemWidget) {
            return m_pSearchItemWidget->nPageIndex();
        }
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

    int m_nStartIndex = 0;
    int m_nEndIndex = 19;

    while (m_isRunning) {

        if (m_nStartIndex < 0) {
            m_nStartIndex = 0;
        }
        if (m_nEndIndex >= m_pages) {
            m_nEndIndex = m_pages - 1;
        }

        int page = -1;

        if (!m_pSearchResWidget) {
            break;
        }
        auto dproxy = DocummentFileHelper::instance();
        if (nullptr == dproxy) {
            break;
        }

        for (int index = m_nStartIndex; index <= m_nEndIndex; index++) {
            if (!m_isRunning) {
                break;
            }
            page = m_pSearchResWidget->getSearchPage(index);

            if (page == -1) {
                continue;
            }

            QImage image;
            bool bl = dproxy ->getImage(page, image, 42, 62/*34, 54*/);
            if (bl) {
                QImage img = Utils::roundImage(QPixmap::fromImage(image), ICON_SMALL);
                emit sigLoadImage(page, img);
                msleep(60);
            }
        }

        m_nStartIndex += FIRST_LOAD_PAGES;
        m_nEndIndex   += FIRST_LOAD_PAGES;

        msleep(30);
    }
}
