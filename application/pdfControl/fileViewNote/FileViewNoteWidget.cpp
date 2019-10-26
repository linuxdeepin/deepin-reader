#include "FileViewNoteWidget.h"

FileViewNoteWidget::FileViewNoteWidget(CustomWidget *parent):
    CustomWidget(QString("FileViewNoteWidget"), parent)
{
    setWindowFlag(Qt::Popup);
    setFixedSize(QSize(200, 300));

    initWidget();
}

int FileViewNoteWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void FileViewNoteWidget::setEditText(const QString &note)
{
    if (m_pTextEdit) {
        m_pTextEdit->clear();
        m_pTextEdit->setText(note);
        m_strNote = note;
    }
}

void FileViewNoteWidget::closeWidget()
{
    slotClosed();
}

void FileViewNoteWidget::showWidget(const int &nPos)
{
    int nWidth = this->width();
    move(nPos - nWidth - 50, 200);
    show();
    raise();
}

// 初始化界面
void FileViewNoteWidget::initWidget()
{
    QPalette plt = this->palette();
    plt.setColor(QPalette::Background, QColor(255, 251, 225));
    this->setPalette(plt);

    m_pCloseLab = new MenuLab;
    m_pCloseLab->setFixedSize(QSize(30, 30));
    m_pCloseLab->setPixmap(QPixmap(QString(":/resources/image/close.svg")));
    connect(m_pCloseLab, SIGNAL(clicked()), this, SLOT(slotClosed()));

    auto m_pHLayoutClose = new QHBoxLayout;
    m_pHLayoutClose->setContentsMargins(1, 0, 0, 0);
    m_pHLayoutClose->addStretch(1);
    m_pHLayoutClose->addWidget(m_pCloseLab);

    m_pTextEdit = new DTextEdit;
    //background color
    QPalette pText = m_pTextEdit->palette();
    pText.setColor(QPalette::Base, QColor(255, 251, 225));
    m_pTextEdit->setPalette(pText);
    //font
    QFont fontContant(QString("SourceHanSansCN-Normal"), 12);
    m_pTextEdit->setFont(fontContant);
    //text corlor
    m_pTextEdit->setTextColor(QColor(QString("#452B0A")));
    //frame style
    m_pTextEdit->setFrameStyle(QFrame::NoFrame);
    //text under line
    QTextCursor cursor(m_pTextEdit->textCursor());
    QTextCharFormat format = cursor.charFormat ();
    QTextBlockFormat textBlockFormat;
    //line height
    textBlockFormat.setLineHeight(19, QTextBlockFormat::FixedHeight);
    //line margin
    textBlockFormat.setBottomMargin(0);
    format.setFontUnderline(format.fontUnderline() == true ? false : true);
    cursor.mergeCharFormat(format);
    cursor.setBlockFormat(textBlockFormat);
    m_pTextEdit->setTextCursor(cursor);
    //line count
    m_pTextEdit->document()->setMaximumBlockCount(10);

    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    m_pVLayout->addItem(m_pHLayoutClose);
    m_pVLayout->addWidget(m_pTextEdit);

    this->setLayout(m_pVLayout);
}

// 关闭槽函数
void FileViewNoteWidget::slotClosed()
{
    QString t_contant = m_pTextEdit->toPlainText().trimmed();
    if (t_contant != m_strNote) {
        m_strNote = t_contant;
        sendMsg(MSG_NOTE_ADDCONTANT, m_strNote);
    }
    this->close();
}

/*************************CustomTextEdit************************************/

CustomTextEdit::CustomTextEdit(DWidget *parent):
    DTextEdit(parent)
{

}

void CustomTextEdit::initWidget()
{

}
