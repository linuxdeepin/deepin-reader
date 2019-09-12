#include "SearchResWidget.h"

SearchResWidget::SearchResWidget(CustomWidget *parent) :
    CustomWidget("SearchResWidget", parent)
{
    connect(this, SIGNAL(sigFlushSearchWidget(QVariant)),
            this, SLOT(slotFlushSearchList(QVariant)));

    initWidget();
}

void SearchResWidget::slotFlushSearchList(QVariant value)
{
    QMap<int, stSearchRes> m_resMap =     value.value<QMap<int, stSearchRes>>();
    qDebug() << "slotFlushSearchList...";
    m_pNotesList->clear();

    int index = 0;
    int resultNum = 0;
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

    sendMsg(MSG_SWITCHLEFTWIDGET, "3");
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

void SearchResWidget::addNotesItem(const QImage &image, const int &page, const QString &text, const int &resultNum)
{
    NotesItemWidget *itemWidget = new NotesItemWidget;

    itemWidget->setLabelImage(image);
    itemWidget->setLabelPage(tr("Page:%1").arg(page + 1));
    itemWidget->setTextEditText(text);

    itemWidget->setSerchResultText(tr("   %1个结果").arg(resultNum));

    itemWidget->setMinimumSize(QSize(250, 150));

    QListWidgetItem *item = new QListWidgetItem(m_pNotesList);
    item->setFlags(Qt::ItemIsSelectable);
    item->setSizeHint(QSize(250, 150));

    m_pNotesList->insertItem(page, item);
    m_pNotesList->setItemWidget(item, itemWidget);
}

int SearchResWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_FIND_CONTENT) {        //  查询内容
        if (msgContent != "") {
            QMap<int, stSearchRes> resMap;
            DocummentProxy::instance()->search(msgContent, resMap, QColor(255, 0, 0));

            QVariant var;
            var.setValue(resMap);
            emit sigFlushSearchWidget(var);

            return ConstantMsg::g_effective_res;
        }
    }

    return 0;
}
