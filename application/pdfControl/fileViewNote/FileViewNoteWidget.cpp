#include "FileViewNoteWidget.h"

FileViewNoteWidget::FileViewNoteWidget(CustomWidget *parent):
    CustomWidget(QString("FileViewNoteWidget"), parent)
{
    //设置窗体无边框
    //setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint|Qt::Tool|Qt::X11BypassWindowManagerHint);

    setWindowFlag(Qt::Popup);
    setFixedSize(QSize(254, 321));

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
    m_pCloseLab->setFixedSize(QSize(24, 24));
    m_pCloseLab->setPixmap(QPixmap(QString(":/resources/image/close.svg")));
    connect(m_pCloseLab, SIGNAL(clicked()), this, SLOT(slotClosed()));

    auto m_pHLayoutClose = new QHBoxLayout;
    m_pHLayoutClose->setContentsMargins(224, 4, 6, 4);
    m_pHLayoutClose->addStretch(0);
    m_pHLayoutClose->addWidget(m_pCloseLab);

    auto m_pHLayoutContant = new QHBoxLayout;
    m_pHLayoutContant->setContentsMargins(25, 0, 24, 0);
    m_pHLayoutContant->addStretch(0);

    m_pTextEdit = new DTextEdit;
    m_pTextEdit->setFixedSize(205, 257);
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
    format.setFontUnderline(true);
    cursor.mergeCharFormat(format);
    cursor.setBlockFormat(textBlockFormat);
    m_pTextEdit->setTextCursor(cursor);
    //line count
    m_pTextEdit->document()->setMaximumBlockCount(10);
    m_pTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(m_pTextEdit, SIGNAL(textChanged()), this, SLOT(slotTextEditMaxContantNum()));

    m_pHLayoutContant->addWidget(m_pTextEdit);

    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    m_pVLayout->addItem(m_pHLayoutClose);
    m_pVLayout->addItem(m_pHLayoutContant);

    this->setLayout(m_pVLayout);
}

void FileViewNoteWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QRectF rectangle(0, 0, (this->width()), (this->height()));

    QPainter painter(this);
    painter.setOpacity(1);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(/*Qt::NoBrush*/QColor(255, 251, 225));
    painter.drawRoundedRect(rectangle, 6, 6);
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

/**
 * @brief FileViewNoteWidget::slotTextEditMaxContantNum
 * TextEdit输入允许输入最长字符串的长度
 */
void FileViewNoteWidget::slotTextEditMaxContantNum()
{
    QString textContent = m_pTextEdit->toPlainText();

    int length = textContent.count();

    int maxLen = 239;
    if(length > maxLen)
     {
         int position = m_pTextEdit->textCursor().position();
         QTextCursor textCursor = m_pTextEdit->textCursor();
         textContent.remove(position - (length - maxLen), length - maxLen);
         m_pTextEdit->setText(textContent);
         textCursor.setPosition(position - (length - maxLen));
         m_pTextEdit->setTextCursor(textCursor);
     }
}
