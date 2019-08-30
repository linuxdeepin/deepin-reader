#include "NotesItemWidget.h"
#include <QDebug>

NotesItemWidget::NotesItemWidget()
{
    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }

    m_pHLayout = new QHBoxLayout;
    //set around distance
    m_pHLayout->setContentsMargins(0, 0, 0, 0);
    m_pHLayout->setSpacing(0);
    this->setLayout(m_pHLayout);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(onShowContextMenu(const QPoint&)));

    initWidget();
}

NotesItemWidget::~NotesItemWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

void NotesItemWidget::contextMenuEvent(QContextMenuEvent *event)
{

}

void NotesItemWidget::onDltNoteItem()
{
    qDebug() << "delet NotesItemWidget";
}

void NotesItemWidget::onCopyContant()
{
    qDebug() << "copy contant";
}

void NotesItemWidget::onShowContextMenu(const QPoint &)
{
    QMenu * t_menu = new QMenu(this);
    QAction *dltAction = t_menu->addAction(tr("删除书签"));
    QAction *copyAction = t_menu->addAction(tr("复制"));
    connect(dltAction, SIGNAL(triggered()), this, SLOT(onDltBookMark()));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(onCopyContant()));
    t_menu->exec(QCursor::pos());
}

void NotesItemWidget::initWidget()
{

}

int NotesItemWidget::update(const QString &)
{
    return 0;
}
