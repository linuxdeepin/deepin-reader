#include "BookMarkItemWidget.h"
#include <QDebug>
#include "translator/PdfControl.h"

BookMarkItemWidget::BookMarkItemWidget(CustomWidget *parent) :
    CustomWidget(QString("BookMarkItemWidget"), parent)
{

    m_pHLayout = new QHBoxLayout;
    //set around distance
    m_pHLayout->setContentsMargins(1, 0, 1, 0);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(slotShowContextMenu(const QPoint &)));

    initWidget();
}

void BookMarkItemWidget::setItemImage(const QImage &image)
{
    m_pPicture->setPixmap(QPixmap::fromImage(image));
}

int BookMarkItemWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void BookMarkItemWidget::slotDltBookMark()
{
    sendMsg(MSG_BOOKMARK_DLTITEM, QString::number(m_nPageNumber));
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
    m_pPicture = new DLabel;
    m_pPage = new DLabel;

    m_pHLayout->setSpacing(1);
    m_pHLayout->addWidget(m_pPicture);
    m_pHLayout->addWidget(m_pPage);
    m_pHLayout->setSpacing(1);

    this->setLayout(m_pHLayout);
}

int BookMarkItemWidget::PageNumber() const
{
    return m_nPageNumber;
}

void BookMarkItemWidget::setPageNumber(int nPageNumber)
{
    m_nPageNumber = nPageNumber;

    QString sPageText = PdfControl::PAGE_PREF + QString("%1").arg( nPageNumber + 1);

    m_pPage->setText(sPageText);
}

