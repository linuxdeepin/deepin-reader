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
    qDebug() << tr("AddBookMark...");
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

    this->setBookMarks(20);

    this->fillContantToList();
}

void BookMarkWidget::keyPressEvent(QKeyEvent *e)
{
    QString key = Utils::getKeyshortcut(e);

    if (key == "Del") {
        dltItem();
        qDebug() << "dlt bookmark item by key";
    }  else {
        // Pass event to CustomWidget continue, otherwise you can't type anything after here. ;)
        CustomWidget::keyPressEvent(e);
    }
}

void BookMarkWidget::dltItem()
{
    if (m_pCurrentItem != nullptr) {
        delete m_pCurrentItem;
        m_pCurrentItem = nullptr;
    }
}

void BookMarkWidget::addBookMarkItem(const QImage &image, const int &page)
{
    BookMarkItemWidget *t_widget = new BookMarkItemWidget;
    t_widget->setItemImage(image);
    t_widget->setPage(tr("page%1").arg(page + 1));
    t_widget->setMinimumSize(QSize(250, 150));

    QListWidgetItem *item = new QListWidgetItem(m_pBookMarkListWidget);
    item->setFlags(Qt::ItemIsSelectable);
    item->setSizeHint(QSize(250, 150));

    m_pBookMarkListWidget->insertItem(page, item);
    m_pBookMarkListWidget->setItemWidget(item, t_widget);
}

void BookMarkWidget::fillContantToList()
{
    for (int page = 0; page < this->bookMarks(); ++page) {
        QImage image(tr(":/resources/image/logo/logo_big.svg"));

        this->addBookMarkItem(image, page);
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
