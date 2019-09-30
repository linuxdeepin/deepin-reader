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

// 初始化界面
void FileViewNoteWidget::initWidget()
{
    m_pCloseLab = new MenuLab;
    m_pCloseLab->setFixedSize(QSize(30, 30));
    m_pCloseLab->setPixmap(QPixmap(QString(":/resources/image/close.svg")));
    connect(m_pCloseLab, SIGNAL(clicked()), this, SLOT(slotClosed()));

    auto m_pHLayoutClose = new QHBoxLayout;
    m_pHLayoutClose->setContentsMargins(1, 0, 0, 0);
    m_pHLayoutClose->addSpacing(1);
    m_pHLayoutClose->addWidget(m_pCloseLab);

    m_pDltLab = new MenuLab;
    m_pDltLab->setFixedSize(QSize(30, 30));
    m_pDltLab->setPixmap(QPixmap(QString(":/resources/image/delete.svg")));
    connect(m_pDltLab, SIGNAL(clicked()), this, SLOT(slotDlted()));

    auto m_pHLayoutDlt = new QHBoxLayout;
    m_pHLayoutDlt->setContentsMargins(0, 0, 1, 0);
    m_pHLayoutDlt->addWidget(m_pDltLab);
    m_pHLayoutDlt->addSpacing(1);

    m_pTextEdit = new DTextEdit;
    m_pTextEdit->setFixedSize(QSize(200,300));

    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    m_pVLayout->addItem(m_pHLayoutClose);
    m_pVLayout->addWidget(m_pTextEdit);
    m_pVLayout->addItem(m_pHLayoutDlt);

    this->setLayout(m_pVLayout);
}

// 关闭槽函数
void FileViewNoteWidget::slotClosed()
{
    //sendMsg closed signal;
    if (QMessageBox::Yes == DMessageBox::question(nullptr,  QString("Save"), QString("Save this note"))){

        QString t_contant = m_pTextEdit->toPlainText().trimmed();
        if( t_contant != QString("")){
            sendMsg(MSG_NOTE_ADDITEM, t_contant);
        }else {
            sendMsg(MSG_NOTE_DLTNOTEITEM, t_contant);
        }
        qDebug() << "             void FileViewNoteWidget       slotClosed()   " << t_contant;
    }
}

// 删除槽函数
void FileViewNoteWidget::slotDlted()
{
    //sendMsg Dlted signal;
    if (QMessageBox::Yes == DMessageBox::question(nullptr,  QString("DELETE"), QString("delete this note"))){

        qDebug() << "             void FileViewNoteWidget      slotDlted()   ";
        m_pTextEdit->clear();

        sendMsg(MSG_NOTE_ADDCONTANT, QString(""));
    }
}
