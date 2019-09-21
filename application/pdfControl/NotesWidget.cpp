#include "NotesWidget.h"

#include "translator/PdfControl.h"

NotesWidget::NotesWidget(CustomWidget *parent) :
    CustomWidget(QString("NotesWidget"), parent)
{
    initWidget();
}

void NotesWidget::initWidget()
{
    QVBoxLayout *m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    m_pNotesList = new DListWidget;
    m_pNotesList->setSpacing(10);
    //设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
    m_pNotesList->setResizeMode(QListWidget::Adjust);

    m_pVLayout->addWidget(m_pNotesList);

    DImageButton *m_pAddNotesBtn = new DImageButton;
    m_pAddNotesBtn->setText(PdfControl::ADD_NOTE);
    m_pAddNotesBtn->setFixedSize(QSize(250, 50));

    m_pVLayout->addWidget(m_pAddNotesBtn);
}

void NotesWidget::addNotesItem(const QImage &image, const int &page, const QString &text)
{
    NotesItemWidget *itemWidget = new NotesItemWidget;

    itemWidget->setPage(page);
    itemWidget->setLabelImage(image);
    itemWidget->setLabelPage((PdfControl::PAGE_PREF + QString("%1").arg(page + 1)));
    itemWidget->setTextEditText(text);
    itemWidget->setMinimumSize(QSize(250, 150));

    QListWidgetItem *item = new QListWidgetItem(m_pNotesList);
    item->setFlags(Qt::ItemIsSelectable);
    item->setSizeHint(QSize(250, 150));

    m_pNotesList->addItem(item);
    m_pNotesList->setItemWidget(item, itemWidget);
}

int NotesWidget::dealWithData(const int &, const QString &)
{
    return 0;
}
