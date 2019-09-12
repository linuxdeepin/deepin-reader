#include "NotesWidget.h"

NotesWidget::NotesWidget(CustomWidget *parent, unsigned int type) :
    CustomWidget("NotesWidget", parent),
    m_widgetType(type)
{
    m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    m_resMap.clear();

    connect(this, SIGNAL(sigFlushSearchWidget()), this, SLOT(slotFlushSearchList()));

    initWidget();
}

void NotesWidget::slotFlushSearchList()
{
    qDebug() << (m_widgetType != SEARCH_WIDGET);
    if (!m_pNotesList || m_widgetType != SEARCH_WIDGET) {
        return;
    }

    qDebug() << "slotFlushSearchList...";
    m_pNotesList->clear();

    unsigned int index = 0;
    unsigned int resultNum = 0;
    QString strText;
    QImage image;

    for (auto it = m_resMap.begin(); it != m_resMap.end(); ++it) {
        qDebug() << it.key();
        int page = it.key();
        foreach (QString strtext, it.value().listtext) {
            qDebug() << strtext;
            strText += strtext + tr("\n");
            ++resultNum;
        }

        DocummentProxy::instance()->getImage(page, image, 113, 143);
        addNotesItem(image, index++, strText, resultNum);
        resultNum = 0;
        strText.clear();
    }

    sendMsg(MSG_SWITCHLEFTWIDGET, tr("%1").arg(SEARCH));
}

void NotesWidget::initWidget()
{
    m_pNotesList = new DListWidget;
    m_pNotesList->setSpacing(10);
    //设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
    m_pNotesList->setResizeMode(QListWidget::Adjust);

    m_pVLayout->addWidget(m_pNotesList);

    if (m_widgetType == NOTE_WIDGET) {
        m_pAddNotesBtn = new DImageButton;
        m_pAddNotesBtn->setText(tr("adding note"));
        m_pAddNotesBtn->setFixedSize(QSize(250, 50));

        m_pVLayout->addWidget(m_pAddNotesBtn);
    }

    if (m_widgetType == NOTE_WIDGET) {
        this->fillContantToList();
    }
}

void NotesWidget::addNotesItem(const QImage &image, const unsigned int &page, const QString &text, const unsigned int resultNum)
{
    NotesItemWidget *itemWidget = new NotesItemWidget;

    itemWidget->setLabelImage(image);
    itemWidget->setLabelPage(tr("Page:%1").arg(page + 1));
    itemWidget->setTextEditText(text);
    if (m_widgetType == SEARCH_WIDGET) {
        itemWidget->setSerchResultText(tr("   %1个结果").arg(resultNum));
    }
    itemWidget->setMinimumSize(QSize(250, 150));

    QListWidgetItem *item = new QListWidgetItem(m_pNotesList);
    item->setFlags(Qt::ItemIsSelectable);
    item->setSizeHint(QSize(250, 150));

    m_pNotesList->insertItem(page, item);
    m_pNotesList->setItemWidget(item, itemWidget);
}

void NotesWidget::fillContantToList()
{
    for (unsigned int page = 0; page < 10; ++page) {
        QImage image(tr(":/resources/image/logo/logo_big.svg"));

        addNotesItem(image, page, tr("something..."), page);
    }
}

int NotesWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_FIND_CONTENT:          //  查询内容
        if ( m_widgetType != SEARCH_WIDGET ) {
            return 0;
        }

        if (msgContent != "") {

            DocummentProxy::instance()->search(msgContent, m_resMap, QColor(255, 0, 0));

            emit sigFlushSearchWidget();

            return ConstantMsg::g_effective_res;
        }
    }

    return 0;
}
