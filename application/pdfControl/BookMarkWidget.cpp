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

    connect(m_pBookMarkListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotShowSelectItem(QListWidgetItem *)));
}

void BookMarkWidget::slotShowSelectItem(QListWidgetItem *item)
{
    m_pCurrentItem = item;
}

void BookMarkWidget::slotAddBookMark()
{
    int page = DocummentProxy::instance()->currentPageNo();

    if (m_booksMap.find(page) != m_booksMap.end()) {
        qDebug() << tr("   page:%1").arg(page + 1) << m_booksMap[page];

        return;
    }

    QImage image;

    DocummentProxy::instance()->getImage(page, image, 130, 150);

    addBookMarkItem(image, page, page);

    m_booksMap.insert(page, true);

    qDebug() << tr("AddNewBookMark...") << tr("   page:%1").arg(page + 1);
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

//    this->setBookMarks(20);
//    this->fillContantToList();
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

void BookMarkWidget::dltItem()
{
    if (m_pCurrentItem != nullptr) {

        BookMarkItemWidget *t_widget = nullptr;
        QStringList t_list;
        QString t_text;
        int page = -1;

        t_widget = reinterpret_cast<BookMarkItemWidget *>(m_pBookMarkListWidget->itemWidget(m_pCurrentItem));

        if (t_widget) {
            t_text = t_widget->pageLabel()->text();
            t_list = t_text.split(":");
            page = t_list[1].toInt();
            qDebug() << page;

            t_widget->destructMember();
        }

        if (m_booksMap.remove(page - 1) > 0) {

            m_pBookMarkListWidget->removeItemWidget(m_pCurrentItem);
            delete m_pCurrentItem;
            m_pCurrentItem = nullptr;

            if (m_booksMap.count() == 0) {
                m_pBookMarkListWidget->clear();
            }
        }
    }
}

void BookMarkWidget::addBookMarkItem(const QImage &image, const int &page, const int &index)
{
    BookMarkItemWidget *t_widget = new BookMarkItemWidget;
    t_widget->setItemImage(image);
    t_widget->setPage(tr("page:%1").arg(page + 1));
    t_widget->setMinimumSize(QSize(250, 150));

    QListWidgetItem *item = new QListWidgetItem(m_pBookMarkListWidget);
    item->setFlags(Qt::ItemIsSelectable);
    item->setSizeHint(QSize(250, 150));

    m_pBookMarkListWidget->insertItem(index, item);
    m_pBookMarkListWidget->setItemWidget(item, t_widget);
}

void BookMarkWidget::fillContantToList()
{
    for (int page = 0; page < this->bookMarks(); ++page) {
        QImage image(tr(":/resources/image/logo/logo_big.svg"));

        this->addBookMarkItem(image, (page + 1), page);
    }
}

int BookMarkWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_BOOKMARK_DLTITEM == msgType) {
        dltItem();
        qDebug() << "dlt bookmark item by menu";
        return ConstantMsg::g_effective_res;
    }

    return 0;
}
