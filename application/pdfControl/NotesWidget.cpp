#include "NotesWidget.h"

NotesWidget::NotesWidget(CustomWidget *parent) :
    CustomWidget("NotesWidget", parent)
{
    m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    initWidget();
}

void NotesWidget::initWidget()
{
    m_pNotesList = new DListWidget;
    m_pNotesList->setSpacing(10);
    //设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
    m_pNotesList->setResizeMode(QListWidget::Adjust);

    m_pAddNotesBtn = new DImageButton;
    m_pAddNotesBtn->setText(tr("adding note"));
    m_pAddNotesBtn->setFixedSize(QSize(250, 50));

    m_pVLayout->addWidget(m_pNotesList);
    m_pVLayout->addWidget(m_pAddNotesBtn);

    this->fillContantToList();
}

void NotesWidget::addNotesItem(const QImage &image, const int &page, const QString &text)
{
    NotesItemWidget *itemWidget = new NotesItemWidget;

    itemWidget->setLabelImage(image);
    itemWidget->setLabelPage(tr(" Page:%1").arg(page + 1));
    itemWidget->setTextEditText(text);
    itemWidget->setMinimumSize(QSize(250, 150));

    QListWidgetItem *item = new QListWidgetItem(m_pNotesList);
    item->setFlags(Qt::ItemIsSelectable);
    item->setSizeHint(QSize(250, 150));

    m_pNotesList->insertItem(page, item);
    m_pNotesList->setItemWidget(item, itemWidget);
}

void NotesWidget::fillContantToList()
{
    for (int page = 0; page < 20; ++page) {
        QImage image(tr(":/resources/image/logo/logo_big.svg"));

        addNotesItem(image, page, tr("something..."));
    }
}

int NotesWidget::dealWithData(const int &, const QString &)
{
    return 0;
}
