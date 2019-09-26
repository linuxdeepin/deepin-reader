#include "NotesItemWidget.h"
#include <QDebug>
#include "translator/PdfControl.h"
#include <DApplication>
#include <QClipboard>

NotesItemWidget::NotesItemWidget(CustomItemWidget *parent) :
    CustomItemWidget(QString("NotesItemWidget"), parent)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(slotShowContextMenu(const QPoint &)));

    initWidget();
}

void NotesItemWidget::setTextEditText(const QString &contant)
{
    if (m_pTextEdit) {
        m_pTextEdit->setText(contant);
    }
}

void NotesItemWidget::setSerchResultText(const QString &result)
{
    if (m_pSearchResultNum) {
        m_pSearchResultNum->setText(result);
    }
}

void NotesItemWidget::slotDltNoteItem()
{
    qDebug() << "delet NotesItemWidget";
}

void NotesItemWidget::slotCopyContant()
{
    qDebug() << "            copy contant          ";
    if (m_pTextEdit) {
        QString str = m_pTextEdit->document()->toPlainText();
        if (str != QString("")) {
            QClipboard *clipboard = DApplication::clipboard();   //获取系统剪贴板指针
            clipboard->setText(str);
        }
    }
}

void NotesItemWidget::slotShowContextMenu(const QPoint &)
{
    QMenu *t_menu = new QMenu(this);
    QAction *copyAction = t_menu->addAction(PdfControl::COPY_CONT);
    QAction *dltItemAction = t_menu->addAction(PdfControl::DLT_NOTE);
    connect(dltItemAction, SIGNAL(triggered()), this, SLOT(slotDltNoteItem()));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(slotCopyContant()));
    t_menu->exec(QCursor::pos());
}

void NotesItemWidget::initWidget()
{
    auto t_vLayout = new QVBoxLayout;
    t_vLayout->setContentsMargins(0, 0, 0, 0);
    t_vLayout->setSpacing(0);

    auto t_hLayout = new QHBoxLayout;
    t_hLayout->setContentsMargins(0, 0, 0, 0);
    t_hLayout->setSpacing(0);

    m_pPicture = new CustomLabel;
    m_pPicture->setFixedSize(QSize(150, 150));

    m_pPage = new DLabel;
    m_pSearchResultNum = new DLabel;
    m_pTextEdit = new DTextEdit;
    m_pTextEdit->setEnabled(false);

    t_hLayout->addWidget(m_pPage);
    t_hLayout->addWidget(m_pSearchResultNum);

    t_vLayout->addItem(t_hLayout);
    t_vLayout->addWidget(m_pTextEdit);

    auto m_pHLayout = new QHBoxLayout;

    m_pHLayout->setContentsMargins(0, 0, 1, 0);
    m_pHLayout->addWidget(m_pPicture);
    m_pHLayout->addItem(t_vLayout);
    m_pHLayout->setSpacing(1);

    this->setLayout(m_pHLayout);
}

int NotesItemWidget::dealWithData(const int &, const QString &)
{
    return 0;
}
