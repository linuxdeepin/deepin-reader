#include "ThumbnailWidget.h"

ThumbnailWidget::ThumbnailWidget(DWidget *parent) :
    DWidget(parent)
{
    resize(250, 250);
    initWidget();

    connect(m_pThumbnailListWidget, SIGNAL(	itemClicked(QListWidgetItem *)), this, SLOT(onShowSelectItem(QListWidgetItem *)));
}

ThumbnailWidget::~ThumbnailWidget()
{

}

void ThumbnailWidget::initWidget()
{
    m_pThumbnailListWidget = new DListWidget(this);
    m_pThumbnailListWidget->setFixedSize(QSize(250, 480));
    //m_pThumbnailListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pThumbnailListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    for (int idex = 0; idex < 30; ++idex) {
        ItemWidget * widget = new ItemWidget(nullptr);

        widget->setContantLabelPixmap(QString(":/images/logo_96.svg"));
        widget->setPageLabelText(QString("               %1").arg(idex + 1));
        widget->setMinimumSize(QSize(250,250));

        QListWidgetItem * item = new QListWidgetItem(m_pThumbnailListWidget);
        item->setFlags(Qt::ItemIsSelectable);
        item->setSizeHint(QSize(250,250));

        m_pThumbnailListWidget->insertItem(idex, item);
        //m_pThumbnailListWidget->addItem(item);
        m_pThumbnailListWidget->setItemWidget(item, widget);
    }
}

void ThumbnailWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    if(m_itemWidget)
    {
//        QPalette pal(m_itemWidget->palette());
//        //设置页面背景白色
//        pal.setColor(QPalette::Background, Qt::white);
//        m_itemWidget->setPalette(pal);
        QColor color = QColor(Qt::color0);
        QPalette pw = m_itemWidget->palette();
        pw.setColor(QPalette::Window,color);
        m_itemWidget->setPalette(pw);

        //设置页码标签字体颜色
        QPalette pl;
        pl.setColor(QPalette::WindowText,Qt::black);
        m_pSonWidgetPageLabel->setPalette(pl);

        qDebug() << "clean background color";
    }
    if(item)
    {
        DWidget * t_widget = nullptr;
        DLabel * t_label = nullptr;
        ItemWidget * t_ItemWidget = (ItemWidget *)(m_pThumbnailListWidget->itemWidget(item));

        t_widget = t_ItemWidget->getSonWidget();
        t_label = t_ItemWidget->getPageLabel();

//        QPalette pal(t_widget->palette());
//        //设置背景红色
//        pal.setColor(QPalette::Background, Qt::red);
//        t_widget->setPalette(pal);

        QColor color = QColor(Qt::darkBlue);
        QPalette pw = t_widget->palette();
        pw.setColor(QPalette::Window,color);
        t_widget->setPalette(pw);

        //设置页码标签字体颜色
        QPalette pl;
        pl.setColor(QPalette::WindowText,Qt::darkBlue);
        t_label->setPalette(pl);

        qDebug() << "set background color";
        m_itemWidget = t_widget;
        m_pSonWidgetPageLabel = t_label;
    }
}

void ThumbnailWidget::onShowSelectItem(QListWidgetItem * item)
{
    setSelectItemBackColor(item);

    int index = m_pThumbnailListWidget->row(item);
    emit selectIndexPage(++index);
    qDebug() << index;
}

void ThumbnailWidget::onSetJumpToPage(const int& index)
{
    QListWidgetItem * item = m_pThumbnailListWidget->item(index);
    setSelectItemBackColor(item);
}

ItemWidget::ItemWidget(DWidget *parent) :
    DWidget(parent)
{
    m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    initWidget();
}

ItemWidget::~ItemWidget()
{
    if(m_sonWidget)
    {
        delete m_sonWidget;
        m_sonWidget = nullptr;
    }
    if(m_pContantLabel)
    {
        delete m_pContantLabel;
        m_pContantLabel = nullptr;
    }
    if(m_pPageLabel)
    {
        delete m_pPageLabel;
        m_pPageLabel = nullptr;
    }
}

void ItemWidget::initWidget()
{
    QGridLayout * gridLayout = new QGridLayout;
    m_sonWidget = new DWidget;
    m_sonWidget->setFixedSize(QSize(150,150));
    m_sonWidget->setAutoFillBackground(true);
    m_sonWidget->setLayout(gridLayout);

    m_pContantLabel = new DLabel(this);
    m_pPageLabel = new DLabel(this);
    m_pPageLabel->setFixedSize(QSize(250, 20));

    m_pContantLabel->setFixedSize(QSize(150,150));
    gridLayout->addWidget(m_pContantLabel);

    m_pVLayout->addWidget(m_sonWidget);
    m_pVLayout->addWidget(m_pPageLabel);
    this->setLayout(m_pVLayout);
}

void ItemWidget::setContantLabelPixmap(QString pix)
{
    m_pContantLabel->setPixmap(QPixmap(pix));
}

void ItemWidget::setPageLabelText(QString text)
{
    QFont ft;
    ft.setPointSize(12);
    m_pPageLabel->setFont(ft);
    m_pPageLabel->setText(text);
}
