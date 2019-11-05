#include "BookMarkItemWidget.h"

BookMarkItemWidget::BookMarkItemWidget(CustomItemWidget *parent) :
    CustomItemWidget("BookMarkItemWidget", parent)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);//让widget支持右键菜单事件
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(slotShowContextMenu(const QPoint &)));

    initWidget();
}
/**
 * @brief BookMarkItemWidget::dealWithData
 * 处理全局消息接口
 * @return
 */
int BookMarkItemWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

/**
 * @brief BookMarkItemWidget::slotDltBookMark
 * 删除当前书签，并发送删除书签全局消息
 */
void BookMarkItemWidget::slotDltBookMark()
{
    sendMsg(MSG_BOOKMARK_DLTITEM, QString::number(m_nPageIndex));
}

/**
 * @brief BookMarkItemWidget::slotShowContextMenu
 * 显示书签右键菜单
 */
void BookMarkItemWidget::slotShowContextMenu(const QPoint &)
{
    QMenu *t_menu = new QMenu(this);

    QAction *dltBookMarkAction = t_menu->addAction(tr("delete bookmark"));
    connect(dltBookMarkAction, SIGNAL(triggered()), this, SLOT(slotDltBookMark()));
    t_menu->exec(QCursor::pos());
}

/**
 * @brief BookMarkItemWidget::initWidget
 * 初始化界面
 */
void BookMarkItemWidget::initWidget()
{
    QFont font(QString("SourceHanSansSC-Medium"), 12);
    m_pPageNumber = new DLabel(this);
    m_pPageNumber->setMinimumWidth(31);
    m_pPageNumber->setFixedHeight(18);
    m_pPageNumber->setFont(font);

    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(20, 18, 0, 44);
    m_pVLayout->addWidget(m_pPageNumber);

    m_pPicture = new CustomLabel(this);
    m_pPicture->setFixedSize(QSize(42, 62));
    m_pPicture->setAlignment(Qt::AlignCenter);

    auto m_pHLayout = new QHBoxLayout;
    m_pHLayout->setContentsMargins(1, 0, 1, 0);
    m_pHLayout->setSpacing(1);
    m_pHLayout->addWidget(m_pPicture);

    m_pHLayout->addItem(m_pVLayout);

    this->setLayout(m_pHLayout);
}

void BookMarkItemWidget::paintEvent(QPaintEvent *event)
{
    QPalette p(m_pPicture->palette());
    QColor color;

    //  涉及到 主题颜色
    if (m_bPaint) {
        color = QColor(QString("#0081FF"));
        p.setColor(QPalette::Text, color);
    } else {
        color = QColor::fromRgbF(0, 0, 0, 0.08);
        p.setColor(QPalette::Text, color);
    }

    m_pPicture->setPalette(p);

    int width = this->width();
    int height = this->height();

    QPainter painter(this);
    painter.setPen(QPen(QColor(QString("#D8D8D8")), 1));

    QPoint startP(67, height-1);
    QPoint endP(width, height-1);

    painter.drawLine(startP, endP);


    CustomWidget::paintEvent(event);
}

