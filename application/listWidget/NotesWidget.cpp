#include "NotesWidget.h"

NotesWidget::NotesWidget(DWidget *parent) :
    DWidget(parent)
{
    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }

    m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    initWidget();
}

NotesWidget::~NotesWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

void NotesWidget::initWidget()
{
    m_pNotesList = new DListWidget;
    m_pVLayout->addWidget(m_pNotesList);

    for (int page = 0; page < 20; ++page) {
        NotesItemWidget * itemWidget = new NotesItemWidget;
        itemWidget->setLabelPage(tr(" Page:%1").arg(page+1));
        itemWidget->setLabelPix(tr(":/images/logo_96.svg"));
        itemWidget->setTextEditText(tr("note something..."));
        itemWidget->setMinimumSize(QSize(250,150));

        QListWidgetItem * item = new QListWidgetItem(m_pNotesList);
        item->setFlags(Qt::ItemIsSelectable);
        item->setSizeHint(QSize(250,150));

        m_pNotesList->insertItem(page, item);
        m_pNotesList->setItemWidget(item, itemWidget);
    }
}

int NotesWidget::update(const QString &)
{
    return 0;
}
