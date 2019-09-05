#include "PagingWidget.h"
#include <QDebug>

PagingWidget::PagingWidget(CustomWidget *parent) :
    CustomWidget("PagingWidget", parent)
{
    resize(250, 20);
    initWidget();
}

void PagingWidget::initWidget()
{
    m_pTotalPagesLab = new DLabel(this);
    m_pTotalPagesLab->setText(QString("/xxx页"));
    m_pTotalPagesLab->setMinimumWidth(80);

    m_pPrePageBtn = new DImageButton(this);
    m_pPrePageBtn->setText(tr("<"));
    m_pPrePageBtn->setFixedSize(QSize(40, 40));

    m_pNextPageBtn = new DImageButton(this);
    m_pNextPageBtn->setText(tr(">"));
    m_pNextPageBtn->setFixedSize(QSize(40, 40));

    connect(m_pPrePageBtn, SIGNAL(clicked()), SLOT(slotPrePage()));
    connect(m_pNextPageBtn, SIGNAL(clicked()), SLOT(slotNextPage()));

    m_pJumpPageSpinBox = new DSpinBox(this);
    m_pJumpPageSpinBox->setRange(1, 100);
    m_pJumpPageSpinBox->setValue(1);
    m_pJumpPageSpinBox->setMinimumWidth(50);
    m_pJumpPageSpinBox->installEventFilter(this);
    m_pJumpPageSpinBox->setWrapping(true);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_pJumpPageSpinBox);
    hLayout->addWidget(m_pTotalPagesLab);
    hLayout->addWidget(m_pPrePageBtn);
    hLayout->addWidget(m_pNextPageBtn);
    this->setLayout(hLayout);

    setTotalPages(30);
}

bool PagingWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_pJumpPageSpinBox) {
        if (event->type() == QEvent::KeyPress) {

            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {

                int index = m_pJumpPageSpinBox->value() - 1;
                if (this->getPreRowVal() != index) {

                    this->setPreRowVal(index);
                    setCurrentPageValue(index);
                    sendMsg(MSG_THUMBNAIL_JUMPTOPAGE, QString::number(index));
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void PagingWidget::setCurrentPage(const int &index)
{
    this->setPreRowVal(index - 1);

    m_pJumpPageSpinBox->setValue(index);

    qDebug() << tr("page: %1").arg(index);
}

void PagingWidget::createBtn(DImageButton *btn, QWidget *parent, const QString &text, const QString &btnName, const QString &normalPic, const QString &hoverPic, const QString &pressPic, const QString &checkedPic, const char *member, bool checkable, bool checked)
{
    btn = new DImageButton(normalPic, hoverPic, pressPic, checkedPic, parent);
    btn->setText(text);
    btn->setFixedSize(QSize(40, 40));
    btn->setToolTip(btnName);
    btn->setCheckable(checkable);

    connect(btn, SIGNAL(checkedChanged()), member);
}

void PagingWidget::setTotalPages(int pages)
{
    m_totalPage = pages;
    m_currntPage = FIRSTPAGES;
    m_pTotalPagesLab->setText(QString("/%1页").arg(pages));

    m_pJumpPageSpinBox->setRange(1, (pages < 1) ? 1 : pages);
}

int PagingWidget::dealWithData(const int &msgType, const QString &msgContant)
{
    return 0;
}

void PagingWidget::slotPrePage()
{
    int t_page = --m_currntPage;

    m_pNextPageBtn->setEnabled(true);
    if (t_page <= FIRSTPAGES) {
        m_pPrePageBtn->setEnabled(false);
    }
    if (t_page < FIRSTPAGES) {
        m_currntPage = FIRSTPAGES;
        return;
    }
    m_currntPage = t_page;

    setCurrentPage(m_currntPage);
    sendMsg(MSG_THUMBNAIL_JUMPTOPAGE, QString::number(m_currntPage - FIRSTPAGES));
}

void PagingWidget::slotNextPage()
{
    int t_page = ++m_currntPage;

    m_pPrePageBtn->setEnabled(true);
    if (t_page >= m_totalPage) {
        m_pNextPageBtn->setEnabled(false);
    }
    if (t_page > m_totalPage) {
        m_currntPage = m_totalPage;
        return;
    }
    m_currntPage = t_page;

    setCurrentPage(m_currntPage);
    sendMsg(MSG_THUMBNAIL_JUMPTOPAGE, QString::number(m_currntPage - FIRSTPAGES));
}

void PagingWidget::setCurrentPageValue(const int &index)
{
    m_currntPage = index + FIRSTPAGES;

    if (m_currntPage <= FIRSTPAGES) {
        m_pPrePageBtn->setEnabled(false);
        m_pNextPageBtn->setEnabled(true);
    } else if (m_currntPage >= m_totalPage && m_totalPage != FIRSTPAGES) {
        m_pPrePageBtn->setEnabled(true);
        m_pNextPageBtn->setEnabled(false);
    } else {
        m_pPrePageBtn->setEnabled(true);
        m_pNextPageBtn->setEnabled(true);
    }

    if (m_totalPage == FIRSTPAGES) {
        m_pPrePageBtn->setEnabled(false);
        m_pNextPageBtn->setEnabled(false);
    }

    setCurrentPage(m_currntPage);
}
