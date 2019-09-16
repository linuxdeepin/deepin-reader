#include "BookMarkWidget.h"
#include <QDebug>
#include "application.h"

BookMarkWidget::BookMarkWidget(CustomWidget *parent) :
    CustomWidget("BookMarkWidget", parent)
{
    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(0);
    this->setLayout(m_pVBoxLayout);

    initWidget();

    initConnection();

    m_pAllPageList.append(0);//测试专用
}

void BookMarkWidget::slotShowSelectItem(QListWidgetItem *item)
{
    BookMarkItemWidget *t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(item));
    if (t_widget) {
        int nnPageNumber = t_widget->PageNumber();
        m_nCurrentPage = nnPageNumber;

        DocummentProxy::instance()->pageJump(nnPageNumber);
    }
}

void BookMarkWidget::slotAddBookMark()
{
    int page = DocummentProxy::instance()->currentPageNo();

    if (m_pAllPageList.contains(page)) {
        return;
    }

    if (m_pAllPageList.size() == 0) {
        //bool rl =
        dApp->dbM->insertBookMark(QString::number(page));
//        m_pAllPageList.append(page);

//        addBookMarkItem(page);
    } else {
        QString sPage = "";
        foreach (int i, m_pAllPageList) {
            sPage += QString::number(page) + tr(",");
        }

        dApp->dbM->updateBookMark(QString::number(page));
    }

    m_pAllPageList.append(page);
    addBookMarkItem(page);
}

//  打开文件成功，　获取该文件的书签数据
void BookMarkWidget::slotOpenFileOk()
{
    connect(DocummentProxy::instance(), SIGNAL(signal_pageChange(int)), this, SLOT(slotDocFilePageChanged(int)));

    QString sAllPages = dApp->dbM->getBookMarks();
    QStringList sPageList = sAllPages.split(",", QString::SkipEmptyParts);
    foreach (QString s, sPageList) {
        int nPage = s.toInt();

        m_pAllPageList.append(nPage);

        addBookMarkItem(nPage);
    }
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

void BookMarkWidget::slotDeleteBookItem(const int &nPage)
{
    int nCount = m_pBookMarkListWidget->count();

    for (int iLoop = 0; iLoop < nCount; iLoop++) {
        QListWidgetItem *pItem = m_pBookMarkListWidget->item(iLoop);
        BookMarkItemWidget *t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(pItem));
        if (t_widget) {
            int nnPageNumber = t_widget->PageNumber();
            if (nnPageNumber == nPage) {
                m_pAllPageList.removeOne(nPage);

                t_widget->deleteLater();
                t_widget = nullptr;

                delete  pItem;

                break;
            }
        }
    }
}

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

    m_pVBoxLayout->addWidget(m_pBookMarkListWidget);
    m_pVBoxLayout->addWidget(m_pAddBookMarkBtn);
}

void BookMarkWidget::keyPressEvent(QKeyEvent *e)
{
    QString key = Utils::getKeyshortcut(e);

    if (key == "Del") {
        dltItem();
//        qDebug() << "dlt bookmark item by key";
    }  else {
        // Pass event to CustomWidget continue, otherwise you can't type anything after here. ;)
        CustomWidget::keyPressEvent(e);
    }
}

void BookMarkWidget::initConnection()
{
    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigDeleteBookItem(const int &)), this, SLOT(slotDeleteBookItem(const int &)));
    connect(m_pBookMarkListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotShowSelectItem(QListWidgetItem *)));
}

void BookMarkWidget::dltItem()
{
    slotDeleteBookItem(m_nCurrentPage);
    m_nCurrentPage = -1;
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

    return 0;
}
