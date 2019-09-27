#include "FileViewNoteWidget.h"

FileViewNoteWidget::FileViewNoteWidget(CustomWidget *parent):
    CustomWidget(QString("FileViewNoteWidget"), parent)
{
    setFixedSize(QSize(300, 400));

    initWidget();
}

int FileViewNoteWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void FileViewNoteWidget::initWidget()
{
    m_pCloseLab = new DLabel;
    m_pCloseLab->setFixedSize(QSize(50, 50));
    m_pCloseLab->setPixmap(QString(":/resources/image/close.svg"));

    auto m_pHLayoutClose = new QHBoxLayout;
    m_pHLayoutClose->setContentsMargins(1, 0, 0, 0);
    m_pHLayoutClose->addSpacing(1);
    m_pHLayoutClose->addWidget(m_pCloseLab);

    m_pDltLab = new DLabel;
    m_pDltLab->setFixedSize(QSize(50, 50));
    m_pDltLab->setPixmap(QString(":/resources/image/delete.svg"));

    auto m_pHLayoutDlt = new QHBoxLayout;
    m_pHLayoutDlt->setContentsMargins(0, 0, 1, 0);
    m_pHLayoutDlt->addWidget(m_pDltLab);
    m_pHLayoutDlt->addSpacing(1);

    m_pTextEdit = new DTextEdit;

    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    m_pVLayout->addItem(m_pHLayoutClose);
    m_pVLayout->addWidget(m_pTextEdit);
    m_pVLayout->addItem(m_pHLayoutDlt);

    this->setLayout(m_pVLayout);
}
