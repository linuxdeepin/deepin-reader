#include "PagingWidget.h"

PagingWidget::PagingWidget(DWidget *parent) :
    DWidget(parent)
{
    resize(250, 20);
    initWidget();

    connect(m_pPrePageBtn, SIGNAL(clicked()), this, SLOT(onPrePage()));
    connect(m_pNextPageBtn, SIGNAL(clicked()), this, SLOT(onNextPage()));
}

PagingWidget::~PagingWidget()
{
    if(m_pTotalPagesLab)
    {
        delete m_pTotalPagesLab;
        m_pTotalPagesLab = nullptr;
    }
    if(m_pPrePageBtn)
    {
        delete m_pPrePageBtn;
        m_pPrePageBtn = nullptr;
    }
    if(m_pNextPageBtn)
    {
        delete m_pNextPageBtn;
        m_pNextPageBtn = nullptr;
    }
    if(m_pJumpPageSpinBox)
    {
        delete m_pJumpPageSpinBox;
        m_pJumpPageSpinBox = nullptr;
    }
}

void PagingWidget::initWidget()
{    
    m_pTotalPagesLab = new DLabel(this);
    m_pTotalPagesLab->setText(QString("/xxx页"));
    m_pTotalPagesLab->setMinimumWidth(80);

    m_pPrePageBtn = new DPushButton(this);
    m_pPrePageBtn->setText(QString("<"));

    m_pNextPageBtn = new DPushButton(this);
    m_pNextPageBtn->setText(QString(">"));

    m_pJumpPageSpinBox = new DSpinBox(this);
    m_pJumpPageSpinBox->setRange(1, 100);
    m_pJumpPageSpinBox->setValue(1);
    m_pJumpPageSpinBox->setMinimumWidth(50);

    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->addWidget(m_pJumpPageSpinBox);
    hLayout->addWidget(m_pTotalPagesLab);
    hLayout->addWidget(m_pPrePageBtn);
    hLayout->addWidget(m_pNextPageBtn);
    this->setLayout(hLayout);

    setTotalPages(30);
}

void PagingWidget::setCurrentPage(const int &index)
{
    m_pJumpPageSpinBox->setValue(index);
}

void PagingWidget::setTotalPages(int pages)
{
    m_totalPage = pages;
    m_pTotalPagesLab->setText(QString("/%1页").arg(pages));

    m_pJumpPageSpinBox->setRange(1, (pages<1)?1:pages);
}

void PagingWidget::onPrePage()
{
    int t_page = --m_currntPage;

    m_pNextPageBtn->setEnabled(true);
    if(t_page <= FIRSTPAGES)
    {
        m_pPrePageBtn->setEnabled(false);
        return;
    }
    m_currntPage = t_page;

    setCurrentPage(m_currntPage);
    emit jumpToIndexPage(m_currntPage);
}

void PagingWidget::onNextPage()
{
    int t_page = ++m_currntPage;

    m_pPrePageBtn->setEnabled(true);
    if(t_page >= m_totalPage)
    {
        m_pNextPageBtn->setEnabled(false);
        return;
    }
    m_currntPage = t_page;

    setCurrentPage(m_currntPage);
    emit jumpToIndexPage(m_currntPage);
}

void PagingWidget::onSetCurrentPage(const int& index)
{
    m_currntPage = index;

    if(m_currntPage <= FIRSTPAGES)
    {
        m_pPrePageBtn->setEnabled(false);
        m_pNextPageBtn->setEnabled(true);
    }else if(m_currntPage >= m_totalPage && m_totalPage != FIRSTPAGES){
        m_pPrePageBtn->setEnabled(true);
        m_pNextPageBtn->setEnabled(false);
    }else{
        m_pPrePageBtn->setEnabled(true);
        m_pNextPageBtn->setEnabled(true);
    }

    if(m_totalPage == FIRSTPAGES)
    {
        m_pPrePageBtn->setEnabled(false);
        m_pNextPageBtn->setEnabled(false);
    }

    setCurrentPage(m_currntPage);
    emit jumpToIndexPage(m_currntPage);
}

//void PagingWidget::keyPressEvent(QKeyEvent *event)
//{
////    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
////    {
////        qDebug() << "emit enter event";
////    }
//}
