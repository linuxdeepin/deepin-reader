#include "NotesWidget.h"

#include "translator/PdfControl.h"

#include <DIconButton>

NotesWidget::NotesWidget(CustomWidget *parent) :
    CustomWidget(QString("NotesWidget"), parent)
{
    initWidget();
}

void NotesWidget::initWidget()
{
    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    m_pNotesList = new CustomListWidget;

    m_pVLayout->addWidget(m_pNotesList);

    connect(this, SIGNAL(sigAddNewNoteItem()), this, SLOT(slotAddNoteItem()));
}

void NotesWidget::slotAddNoteItem()
{
    qDebug() << "           NotesWidget::slotAddNoteItem               ";
    QImage image;

    int t_page = DocummentProxy::instance()->currentPageNo();
    DocummentProxy::instance()->getImage(t_page, image, 150, 150);

    addNotesItem(image, t_page, "");
}

void NotesWidget::addNotesItem(const QImage &image, const int &page, const QString &text)
{
    NotesItemWidget *itemWidget = new NotesItemWidget;
    itemWidget->setLabelImage(image);
    itemWidget->setNoteUUid(QString::number(m_nUUid));
    itemWidget->setLabelPage(page, 1);
    itemWidget->setTextEditText(text);
    itemWidget->setMinimumSize(QSize(250, 150));

    QListWidgetItem *item = new QListWidgetItem(m_pNotesList);
    item->setFlags(Qt::NoItemFlags);
    item->setSizeHint(QSize(250, 150));

    m_pNotesList->addItem(item);
    m_pNotesList->setItemWidget(item, itemWidget);

    ++m_nUUid;//测试专用
}

int NotesWidget::dealWithData(const int &, const QString &)
{
    return 0;
}
