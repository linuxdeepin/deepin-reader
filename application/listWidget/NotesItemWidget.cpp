#include "NotesItemWidget.h"
#include <QDebug>


NotesItemWidget::NotesItemWidget(CustomWidget *parent) :
    CustomWidget ("NotesItemWidget", parent)
{
    m_pHLayout = new QHBoxLayout;
    //set around distance
    m_pHLayout->setContentsMargins(0, 0, 0, 0);
    m_pHLayout->setSpacing(0);
    this->setLayout(m_pHLayout);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(slotShowContextMenu(const QPoint &)));

    initWidget();
}

void NotesItemWidget::setLabelPix(const QString &pix)
{
    m_pPicture->setPixmap(QPixmap(pix));
}

void NotesItemWidget::setLabelPage(const QString &value)
{
    QFont ft;
    ft.setPointSize(12);
    m_pPage->setFont(ft);
    m_pPage->setText(value);
}

void NotesItemWidget::setTextEditText(const QString &contant)
{
    m_pTextEdit->setText(contant);
}

void NotesItemWidget::slotDltNoteItem()
{
    qDebug() << "delet NotesItemWidget";
}

void NotesItemWidget::slotCopyContant()
{
    qDebug() << "copy contant";
}

void NotesItemWidget::slotShowContextMenu(const QPoint &)
{
    QMenu *t_menu = new QMenu(this);
    QAction *copyAction = t_menu->addAction(tr("复制"));
    QAction *dltItemAction = t_menu->addAction(tr("删除书签"));
    connect(dltItemAction, SIGNAL(triggered()), this, SLOT(slotDltNoteItem()));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(slotCopyContant()));
    t_menu->exec(QCursor::pos());
}

void NotesItemWidget::initWidget()
{
    QVBoxLayout *t_vLayout = new QVBoxLayout;
    t_vLayout->setContentsMargins(0, 0, 0, 0);
    t_vLayout->setSpacing(0);

    m_pPicture = new  DLabel;
    m_pPage = new  DLabel;
    m_pTextEdit = new DTextEdit;
    m_pTextEdit->setEnabled(false);

    t_vLayout->addWidget(m_pPage);
    t_vLayout->addWidget(m_pTextEdit);

    m_pHLayout->addWidget(m_pPicture);
    m_pHLayout->addLayout(t_vLayout);
}

int NotesItemWidget::dealWithData(const int &, const QString &)
{
    return 0;
}
