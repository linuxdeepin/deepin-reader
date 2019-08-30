#include "BookMarkItemWidget.h"
#include <QDebug>


BookMarkItemWidget::BookMarkItemWidget(DWidget *parent) :
    DWidget (parent)
{
    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }


    m_pHLayout =new QHBoxLayout;
    //set around distance
    m_pHLayout->setContentsMargins(0, 0, 0, 0);
    m_pHLayout->setSpacing(0);
    this->setLayout(m_pHLayout);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(onShowContextMenu(const QPoint&)));

    initWidget();
}

BookMarkItemWidget::~BookMarkItemWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

void BookMarkItemWidget::setPicture(const QString &pix)
{
    m_pPicture->setPixmap(QPixmap(pix));
}

void BookMarkItemWidget::setPage(const QString &value)
{
    QFont ft;
    ft.setPointSize(12);
    m_pPage->setFont(ft);
    m_pPage->setText(value);
}

int BookMarkItemWidget::update(const QString &)
{
    return 0;
}

void BookMarkItemWidget::onDltBookMark()
{
    qDebug() << "delet BookMark";
}

void BookMarkItemWidget::onShowContextMenu(const QPoint &point)
{
    QMenu * t_menu = new QMenu(this);
    QAction *dltBookMarkAction = t_menu->addAction(tr("delete BookMark"));
    connect(dltBookMarkAction, SIGNAL(triggered()), this, SLOT(onDltBookMark()));
    t_menu->exec(QCursor::pos());
}

void BookMarkItemWidget::initWidget()
{
    m_pPicture = new DLabel;
    m_pPage = new DLabel;

    m_pHLayout->addWidget(m_pPicture);
    m_pHLayout->addWidget(m_pPage);
}

