#include "ThumbnailWidget.h"
#include "ThumbnailItemWidget.h"
/*
 *
 */
ThumbnailWidget::ThumbnailWidget(DWidget *parent) :
    DWidget(parent)
{
    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }

    m_pvBoxLayout =new QVBoxLayout;
    m_pvBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pvBoxLayout->setSpacing(0);
    this->setLayout(m_pvBoxLayout);

    initWidget();

    connect(m_pThumbnailListWidget, SIGNAL(	itemClicked(QListWidgetItem *)), this, SLOT(slotShowSelectItem(QListWidgetItem *)));
    connect(m_pPageWidget, SIGNAL(jumpToIndexPage(const int&)), this, SLOT(slotSetJumpToPage(const int&)));
}

ThumbnailWidget::~ThumbnailWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

int ThumbnailWidget::update(const QString &)
{
    return 0;
}

void ThumbnailWidget::initWidget()
{
    m_pThumbnailListWidget = new DListWidget;
    m_pvBoxLayout->addWidget(m_pThumbnailListWidget);
    //m_pThumbnailListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //m_pThumbnailListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_pPageWidget = new PagingWidget;
    m_pvBoxLayout->addWidget(m_pPageWidget);

    for (int idex = 0; idex < 30; ++idex) {
        ThumbnailItemWidget * widget = new ThumbnailItemWidget;
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
    if(item)
    {
        DWidget * t_widget = nullptr;
        DLabel * t_label = nullptr;
        ThumbnailItemWidget * t_ItemWidget = (ThumbnailItemWidget *)(m_pThumbnailListWidget->itemWidget(item));
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

void ThumbnailWidget::slotShowSelectItem(QListWidgetItem * item)
{
    setSelectItemBackColor(item);

    int index = m_pThumbnailListWidget->row(item);
    emit sigSelectIndexPage(++index);
    if(m_pPageWidget){
        m_pPageWidget->setCurrentPageValue(index);
    }
    qDebug() << index;
}

void ThumbnailWidget::slotSetJumpToPage(const int& index)
{
    QListWidgetItem * item = m_pThumbnailListWidget->item(index);
    setSelectItemBackColor(item);
}
