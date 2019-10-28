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
        qDebug() << "set TextEdit text:" << note;
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
    plt.setColor(QPalette::Background, /*QColor(255, 251, 225)*/QColor(QString("#FFFBE1")));
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
    m_pHLayoutContant->setContentsMargins(25, 0, 24, 29);
    m_pHLayoutContant->addStretch(0);

    m_pTextEdit = new CustemTextEdit(this);
#if 0
//    m_pTextEdit->setFixedSize(205, 257);
//    //background color
//    QPalette pText = m_pTextEdit->palette();
//    pText.setColor(QPalette::Base, QColor(255, 251, 225));
//    m_pTextEdit->setPalette(pText);
//    //font
//    QFont fontContant(QString("SourceHanSansCN-Normal"), 12);
//    m_pTextEdit->setFont(fontContant);
//    //text corlor
//    m_pTextEdit->setTextColor(QColor(QString("#452B0A")));
//    //frame style
//    m_pTextEdit->setFrameStyle(QFrame::NoFrame);
//    //text under line
//    QTextCursor cursor(m_pTextEdit->textCursor());
//    QTextCharFormat format = cursor.charFormat ();
//    QTextBlockFormat textBlockFormat;
//    //line height
//    textBlockFormat.setLineHeight(19, QTextBlockFormat::FixedHeight);
//    //line margin
//    textBlockFormat.setBottomMargin(0);
//    format.setFontUnderline(true);
//    cursor.mergeCharFormat(format);
//    cursor.setBlockFormat(textBlockFormat);
//    m_pTextEdit->setTextCursor(cursor);
//    //line count
//    m_pTextEdit->document()->setMaximumBlockCount(10);
//    m_pTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    m_pTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    connect(m_pTextEdit, SIGNAL(textChanged()), this, SLOT(slotTextEditMaxContantNum()));
#endif

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
    painter.setBrush(/*QColor(255, 251, 225)*/QColor(QString("#FFFBE1")));
    painter.drawRoundedRect(rectangle, 6, 6);

    CustomWidget::paintEvent(e);
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

/**************************CustemTextEdit********************************/

CustemTextEdit::CustemTextEdit(DWidget *parent) :
    DTextEdit(parent)
{
    setFixedSize(205, 257);
    init();
}

void CustemTextEdit::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing, true );
    const int w = this->width();
    const int h = this->height();
    const int stepH = 19;
//    QRect rect = this->geometry();

    int x = 0;
    int y = 0;

    QPointF points[] = {
        QPointF(0+x, 0+y),
        QPointF(w+x, 0+y),

        QPointF(0+x, stepH+y),
        QPointF(w+x, stepH+y),

        QPointF(0+x, stepH*2+y),
        QPointF(w+x, stepH*2+y),

        QPointF(0+x, stepH*3+y),
        QPointF(w+x, stepH*3+y),

        QPointF(0+x, stepH*4+y),
        QPointF(w+x, stepH*4+y),

        QPointF(0+x, stepH*5+y),
        QPointF(w+x, stepH*5+y),

        QPointF(0+x, stepH*6+y),
        QPointF(w+x, stepH*6+y),

        QPointF(0+x, stepH*7+y),
        QPointF(w+x, stepH*7+y),

        QPointF(0+x, stepH*8+y),
        QPointF(w+x, stepH*8+y),

        QPointF(0+x, stepH*9+y),
        QPointF(w+x, stepH*9+y),

        QPointF(0+x, h+y),
        QPointF(w+x, h+y)
    };

    QPen pen;
    pen.setColor(QColor(QString("#DBBD77")));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLine(points[0], points[1]);
    painter.drawLine(points[20], points[21]);

    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawLine(points[2], points[3]);
    painter.drawLine(points[4], points[5]);
    painter.drawLine(points[6], points[7]);
    painter.drawLine(points[8], points[9]);
    painter.drawLine(points[10], points[11]);
    painter.drawLine(points[12], points[13]);
    painter.drawLine(points[14], points[15]);
    painter.drawLine(points[16], points[17]);
    painter.drawLine(points[18], points[19]);

    DTextEdit::paintEvent(e);

    this->update();
}

void CustemTextEdit::init()
{
    //background color
    QPalette pText = this->palette();
    pText.setColor(QPalette::Base, QColor(255, 251, 225));
    this->setPalette(pText);
    //font
    QFont fontContant(QString("SourceHanSansCN-Normal"), 12);
    this->setFont(fontContant);
    //text corlor
    this->setTextColor(QColor(QString("#452B0A")));
    //frame style
    this->setFrameStyle(QFrame::NoFrame);
    //text under line
    QTextCursor cursor(this->textCursor());
    QTextCharFormat format = cursor.charFormat ();
    QTextBlockFormat textBlockFormat;
    //line height
    textBlockFormat.setLineHeight(19, QTextBlockFormat::FixedHeight);
    //line margin
    textBlockFormat.setBottomMargin(0);
//    format.setFontUnderline(true);
    cursor.mergeCharFormat(format);
    cursor.setBlockFormat(textBlockFormat);
    this->setTextCursor(cursor);
    //line count
    this->document()->setMaximumBlockCount(10);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextEditMaxContantNum()));
}

/**
 * @brief CustemTextEdit::slotTextEditMaxContantNum
 * TextEdit输入允许输入最长字符串的长度
 */
void CustemTextEdit::slotTextEditMaxContantNum()
{
    QString textContent = this->toPlainText();

    int length = textContent.count();

    if(length > m_nMaxContantLen)
     {
         int position = this->textCursor().position();
         QTextCursor textCursor = this->textCursor();
         textContent.remove(position - (length - m_nMaxContantLen), length - m_nMaxContantLen);
         this->setText(textContent);
         textCursor.setPosition(position - (length - m_nMaxContantLen));
         this->setTextCursor(textCursor);
     }
}
