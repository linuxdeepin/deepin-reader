#include "ThumbnailWidget.h"
#include "ThumbnailItemWidget.h"
/*
 *
 */
ThumbnailWidget::ThumbnailWidget(CustomWidget *parent) :
    CustomWidget(parent)
{
    m_pvBoxLayout = new QVBoxLayout;
    m_pvBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pvBoxLayout->setSpacing(0);
    this->setLayout(m_pvBoxLayout);

    initWidget();

    connect(m_pThumbnailListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotShowSelectItem(QListWidgetItem *)));
}

int ThumbnailWidget::update(const int &msgType, const QString &msgContant)
{
    if (MSG_THUMBNAIL_JUMPTOPAGE == msgType) {
        int row = msgContant.toInt();

        setCurrentRow(row);

        return ConstantMsg::g_effective_res;
    }

    return 0;
}

void ThumbnailWidget::initWidget()
{
    m_pThumbnailListWidget = new DListWidget;
    m_pThumbnailListWidget->setSpacing(10);
    //设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
    m_pThumbnailListWidget->setResizeMode(QListWidget::Adjust);
    m_pvBoxLayout->addWidget(m_pThumbnailListWidget);
    //m_pThumbnailListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //m_pThumbnailListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_pPageWidget = new PagingWidget;
    m_pvBoxLayout->addWidget(m_pPageWidget);

    for (int idex = 0; idex < 30; ++idex) {
        ThumbnailItemWidget *widget = new ThumbnailItemWidget;
        widget->setContantLabelPixmap(QString(":/images/logo_96.svg"));
        widget->setPageLabelText(QString("               %1").arg(idex + 1));
        widget->setMinimumSize(QSize(250, 250));

        QListWidgetItem *item = new QListWidgetItem(m_pThumbnailListWidget);
        item->setFlags(Qt::NoItemFlags);
        item->setFlags(Qt::ItemIsSelectable);
        item->setSizeHint(QSize(250, 250));

        m_pThumbnailListWidget->insertItem(idex, item);
        //m_pThumbnailListWidget->addItem(item);
        m_pThumbnailListWidget->setItemWidget(item, widget);
    }
}

void ThumbnailWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    if (m_itemWidget) {
        m_pThumbnailItemWidget->setPaint(false);
//1
//        QPalette pal(m_itemWidget->palette());
//        //设置页面背景白色
//        pal.setColor(QPalette::Background, Qt::white);
//        m_itemWidget->setPalette(pal);

//2
//        QColor color = QColor(Qt::color0);
//        QPalette pw = m_itemWidget->palette();
//        pw.setColor(QPalette::Window,color);
//        m_itemWidget->setPalette(pw);

//        //设置页码标签字体颜色
//        QPalette pl;
//        pl.setColor(QPalette::WindowText,Qt::black);
//        m_pSonWidgetPageLabel->setPalette(pl);

        //qDebug() << "clean background color";
    }
    if (item) {
        DWidget *t_widget = nullptr;
        DLabel *t_label = nullptr;
        ThumbnailItemWidget *t_ItemWidget = (ThumbnailItemWidget *)(m_pThumbnailListWidget->itemWidget(item));
        t_ItemWidget->setPaint(true);

        t_widget = t_ItemWidget->getSonWidget();
        t_label = t_ItemWidget->getPageLabel();

//        QPalette pal(t_widget->palette());
//        //设置背景红色
//        pal.setColor(QPalette::Background, Qt::red);
//        t_widget->setPalette(pal);

//        QColor color = QColor(Qt::darkBlue);
//        QPalette pw = t_widget->palette();
//        pw.setColor(QPalette::Window,color);
//        t_widget->setPalette(pw);

//        //设置页码标签字体颜色
//        QPalette pl;
//        pl.setColor(QPalette::WindowText,Qt::darkBlue);
//        t_label->setPalette(pl);

        // qDebug() << "set background color";
        m_itemWidget = t_widget;
        m_pSonWidgetPageLabel = t_label;
        m_pThumbnailItemWidget = t_ItemWidget;
    }
}

void ThumbnailWidget::setCurrentRow(const int &row)
{
    m_pThumbnailListWidget->setCurrentRow(row);
}

void ThumbnailWidget::slotShowSelectItem(QListWidgetItem *item)
{
    setSelectItemBackColor(item);

    int row = m_pThumbnailListWidget->row(item);

    m_pMsgSubject->sendMsg(MSG_THUMBNAIL_JUMPTOPAGE, QString::number(row));

    if (m_pPageWidget) {
        m_pPageWidget->setCurrentPageValue(row);
    }
}

void ThumbnailWidget::slotSetJumpToPage(const int &index)
{
    QListWidgetItem *item = m_pThumbnailListWidget->item(index);
    setSelectItemBackColor(item);
}
