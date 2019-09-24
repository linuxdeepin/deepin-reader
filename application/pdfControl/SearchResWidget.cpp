#include "SearchResWidget.h"
#include "translator/PdfControl.h"

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

void SearchResWidget::slotFlushSearchList(QVariant value)
{
    QMap<int, stSearchRes> m_resMap = value.value<QMap<int, stSearchRes>>();
    m_pNotesList->clear();

    int resultNum = 0;
    QString strText;

    for (auto it = m_resMap.begin(); it != m_resMap.end(); ++it) {
        int page = it.key();
        foreach (QString strtext, it.value().listtext) {

            strText += strtext;
            strText += QString("\n");

            ++resultNum;
        }

        addNotesItem(page, strText, resultNum);
        resultNum = 0;
        strText.clear();
    }

    m_loadSearchResThread.setPages(m_resMap.count());
    m_loadSearchResThread.setRunning(true);
    m_loadSearchResThread.start();

    sendMsg(MSG_SWITCHLEFTWIDGET, QString("3"));

}

void SearchResWidget::slotClearWidget()
{
    m_pNotesList->clear();
}

void SearchResWidget::initWidget()
{
    QVBoxLayout *m_pVLayout  = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    m_pNotesList = new DListWidget;
    m_pNotesList->setSpacing(10);
    //设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
    m_pNotesList->setResizeMode(QListWidget::Adjust);

    m_pVLayout->addWidget(m_pNotesList);
}

void SearchResWidget::initConnections()
{
    connect(this, SIGNAL(sigClearWidget()), this, SLOT(slotClearWidget()));
    connect(this, SIGNAL(sigFlushSearchWidget(QVariant)),
            this, SLOT(slotFlushSearchList(QVariant)));
}

void SearchResWidget::addNotesItem(const int &page, const QString &text, const int &resultNum)
{
    NotesItemWidget *itemWidget = new NotesItemWidget;

    itemWidget->setLabelPage(PdfControl::PAGE_PREF + QString("%1").arg(page + 1));
    itemWidget->setTextEditText(text);
    itemWidget->setPage(page);
    itemWidget->setSerchResultText((QString("   %1").arg(resultNum) + PdfControl::SEARCH_RES_CONT));
    itemWidget->setMinimumSize(QSize(250, 150));

    QListWidgetItem *item = new QListWidgetItem(m_pNotesList);
    item->setFlags(Qt::ItemIsSelectable);
    item->setSizeHint(QSize(250, 150));

    m_pNotesList->addItem(item);
    m_pNotesList->setItemWidget(item, itemWidget);
}

int SearchResWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_FIND_CONTENT) {        //  查询内容
        if (msgContent != QString("")) {
            QMap<int, stSearchRes> resMap;
            DocummentProxy::instance()->search(msgContent, resMap, QColor(255, 0, 0));

            QVariant var;
            var.setValue(resMap);
            emit sigFlushSearchWidget(var);

            return ConstantMsg::g_effective_res;
        }
    }

    if (msgType == MSG_FIND_PREV) {
        DocummentProxy::instance()->findPrev();
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_FIND_NEXT) {
        DocummentProxy::instance()->findNext();
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_CLEAR_FIND_CONTENT) {
        emit sigClearWidget();
        DocummentProxy::instance()->clearsearch();
        return ConstantMsg::g_effective_res;
    }

    return 0;
}

int SearchResWidget::getSearchPage(const int &index)
{
    QListWidgetItem *pItem = m_pNotesList->item(index);
    m_pSearchItemWidget = reinterpret_cast<NotesItemWidget *>(m_pNotesList->itemWidget(pItem));
    if (m_pSearchItemWidget) {
        return m_pSearchItemWidget->page();
    }

    return -1;
}

int SearchResWidget::setSearchItemImage(const QImage &image)
{
    if (m_pSearchItemWidget) {
        m_pSearchItemWidget->setLabelImage(image);
        m_pSearchItemWidget = nullptr;
    }

    return 0;
}

/************************************LoadSearchResList*******************************************************/
/************************************加载搜索列表缩略图*********************************************************/

LoadSearchResThread::LoadSearchResThread(QThread *parent):
    QThread(parent)
{

}

void LoadSearchResThread::stopThread()
{
    m_isRunning = false;

    terminate();    //终止线程
    wait();         //阻塞等待
}

void LoadSearchResThread::run()
{
    while (m_isRunning) {

        if (m_nStartIndex < 0) {
            m_nStartIndex = 0;
        }
        if (m_nEndIndex >= m_pages) {
            m_isRunning = false;
            m_nEndIndex = m_pages - 1;
        }

        QImage image;
        int page = -1;

        for (int index = m_nStartIndex; index <= m_nEndIndex; index++) {

            if (!m_pSearchResWidget) {
                continue;
            }

            page = m_pSearchResWidget->getSearchPage(index);

            if (page == -1) {
                continue;
            }

            bool bl = DocummentProxy::instance()->getImage(page, image, 113, 143);

            if (bl) {
                m_pSearchResWidget->setSearchItemImage(image);
            }
        }

        m_nStartIndex += FIRST_LOAD_PAGES;
        m_nEndIndex   += FIRST_LOAD_PAGES;

        msleep(50);
    }
}
