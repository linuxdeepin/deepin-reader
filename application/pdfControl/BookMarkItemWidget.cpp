#include "BookMarkItemWidget.h"
#include "translator/PdfControl.h"

BookMarkItemWidget::BookMarkItemWidget(CustomItemWidget *parent) :
    CustomItemWidget("BookMarkItemWidget", parent)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(slotShowContextMenu(const QPoint &)));

    initWidget();
}

int BookMarkItemWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void BookMarkItemWidget::slotDltBookMark()
{
    sendMsg(MSG_BOOKMARK_DLTITEM, QString::number(m_nPageIndex));
}

void BookMarkItemWidget::slotShowContextMenu(const QPoint &)
{
    QMenu *t_menu = new QMenu(this);

    QAction *dltBookMarkAction = t_menu->addAction(PdfControl::DLT_BK);
    connect(dltBookMarkAction, SIGNAL(triggered()), this, SLOT(slotDltBookMark()));
    t_menu->exec(QCursor::pos());
}

void BookMarkItemWidget::initWidget()
{
    m_pPicture = new CustomLabel;
    m_pPage = new DLabel;

    auto m_pHLayout = new QHBoxLayout;
    m_pHLayout->setContentsMargins(1, 0, 1, 0);
    m_pHLayout->setSpacing(1);

    m_pHLayout->addWidget(m_pPicture);
    m_pHLayout->addWidget(m_pPage);

    this->setLayout(m_pHLayout);
}

