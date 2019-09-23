#include "FileViewNoteWidget.h"

FileViewNoteWidget::FileViewNoteWidget(CustomWidget *parent):
    CustomWidget(QString("FileViewNoteWidget"), parent)
{
    resize(300, 400);
}

int FileViewNoteWidget::dealWithData(const int &, const QString &)
{

    return 0;
}

void FileViewNoteWidget::initWidget()
{
    m_pTextEdit = new QTextEdit;
    m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(1, 1, 1, 1);
    m_pVLayout->setSpacing(1);
    m_pVLayout->addWidget(m_pTextEdit);

    this->setLayout(m_pVLayout);
}
