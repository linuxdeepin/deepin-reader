#include "FileViewNoteWidget.h"
#include "utils/PublicFunction.h"
#include "utils/utils.h"
#include <QTextCodec>
#include <DPlatformWindowHandle>
#include "frame/DocummentFileHelper.h"
#include <DFontSizeManager>

FileViewNoteWidget::FileViewNoteWidget(CustomWidget *parent):
    CustomWidget(QString("FileViewNoteWidget"), parent)
{
    setWindowFlag(Qt::Popup);
    setFixedSize(QSize(250, 320));
    DPlatformWindowHandle handle(this);
    int radius = 18;
    handle.setWindowRadius(radius);

    initWidget();
    initConnections();
    slotUpdateTheme();
}

int FileViewNoteWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }
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

void FileViewNoteWidget::showWidget(const QPoint &point)
{
    move(point);
    show();
    raise();
}

void FileViewNoteWidget::hideEvent(QHideEvent *event)
{
    //  原来是有注释的, 被删除了
    QString t_contant = m_pTextEdit->toPlainText().trimmed();
    if (m_strNote != "" && t_contant == "") {
        sendMsg(MSG_NOTE_DLTNOTECONTANT, m_pNoteUuid);
        m_strNote = t_contant;
    } else {
        QString t_contant = m_pTextEdit->toPlainText().trimmed();       //  注释内容
        if (t_contant != m_strNote) {   //  只有 和 原来已有注释内容不一样, 才会提示 保存
            QString msgContent = "";
            if (m_pNoteUuid != "") {    //  已经高亮
                msgContent = t_contant  + Constant::sQStringSep + m_pNoteUuid + Constant::sQStringSep + m_pNotePage;
            } else {
                msgContent = t_contant + Constant::sQStringSep + m_pHighLightPointAndPage;
            }
            sendMsg(MSG_NOTE_ADDCONTANT, msgContent);
        }
    }
    CustomWidget::hideEvent(event);
}

// 初始化界面
void FileViewNoteWidget::initWidget()
{
    QPalette plt = this->palette();
    plt.setColor(QPalette::Background, QColor(QString("#FFFBE1")));
    this->setPalette(plt);

    m_pCloseLab = new CustomClickLabel("");
    m_pCloseLab->setFixedSize(QSize(24, 24));
    connect(m_pCloseLab, SIGNAL(clicked()), this, SLOT(slotClosed()));

    auto m_pHLayoutClose = new QHBoxLayout;
    m_pHLayoutClose->setContentsMargins(224, 4, 6, 4);
    m_pHLayoutClose->addStretch(0);
    m_pHLayoutClose->addWidget(m_pCloseLab);

    auto m_pHLayoutContant = new QHBoxLayout;
    m_pHLayoutContant->setContentsMargins(25, 0, 24, 29);
    m_pHLayoutContant->addStretch(0);

    m_pTextEdit = new CustemTextEdit(this);
    connect(m_pTextEdit, &CustemTextEdit::sigShowTips, this, &FileViewNoteWidget::slotShowTips);

    m_pHLayoutContant->addWidget(m_pTextEdit);

    auto m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    m_pVLayout->addItem(m_pHLayoutClose);
    m_pVLayout->addItem(m_pHLayoutContant);

    this->setLayout(m_pVLayout);
}

void FileViewNoteWidget::initConnections()
{
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
}

//  主题变了
void FileViewNoteWidget::slotUpdateTheme()
{
    QString sClose = PF::getImagePath("close", Pri::g_pdfControl);
    m_pCloseLab->setPixmap(Utils::renderSVG(sClose, QSize(24, 24)));
}

// 关闭槽函数
void FileViewNoteWidget::slotClosed()
{
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
    if (length > maxLen) {
        int position = m_pTextEdit->textCursor().position();
        QTextCursor textCursor = m_pTextEdit->textCursor();
        textContent.remove(position - (length - maxLen), length - maxLen);
        m_pTextEdit->setText(textContent);
        textCursor.setPosition(position - (length - maxLen));
        m_pTextEdit->setTextCursor(textCursor);
    }
}

void FileViewNoteWidget::slotShowTips()
{
    notifyMsg(MSG_NOTIFY_SHOW_TIP, tr("input limit reached"));
}

void FileViewNoteWidget::setNotePage(const QString &pNotePage)
{
    m_pNotePage = pNotePage;
}

void FileViewNoteWidget::setNoteUuid(const QString &pNoteUuid)
{
    m_pNoteUuid = pNoteUuid;
}

void FileViewNoteWidget::setPointAndPage(const QString &pointAndPage)
{
    m_pHighLightPointAndPage = pointAndPage;
}

/**************************CustemTextEdit********************************/

CustemTextEdit::CustemTextEdit(DWidget *parent) :
    DTextEdit(parent)
{
    setFixedSize(205, 257);
    init();
}
void CustemTextEdit::init()
{
    //background color
    QPalette pText = this->palette();
    pText.setColor(QPalette::Base, QColor(255, 251, 225));
    this->setPalette(pText);

    //font
//    QFont fontContant = Utils::getPixFont(QString("SourceHanSansCN-Normal"), 12);
//    this->setFont(fontContant);
    DFontSizeManager::instance()->bind(this, DFontSizeManager::T8);

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
    cursor.mergeCharFormat(format);
    cursor.setBlockFormat(textBlockFormat);
    this->setTextCursor(cursor);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextEditMaxContantNum()));
}

/**
 * @brief CustemTextEdit::calcTextSize
 * 将Unicode转换成GBK编码计算字节数
 * @param text
 * @return
 */
int CustemTextEdit::calcTextSize(const QString &text)
{
    QByteArray ba = QTextCodec::codecForName(("GBK"))->fromUnicode(text);//unicodezhuangGBK之间的转换器

    const char *str_2 = ba.data();

    int str_len = strlen(str_2);//获取转换长度

    return str_len;
}

QString CustemTextEdit::getMaxLenStr(QString text)
{
    QString qstrChinese = "";
     QString qstrText =  text;

//    for(int index = 0; index < text.count(); ++index)
//    {
//        qstrChinese.append(text.at(index));

//        if(qstrChinese.toUtf8().size() >= m_nMaxContantLen){
//            return qstrChinese;
//        }
//    }
    foreach(QChar ch, qstrText)
    {
        qDebug() << ch;
        qstrChinese.append(ch);

        if(qstrChinese.toUtf8().size() >= m_nMaxContantLen){
            return qstrChinese;
        }
    }

    return qstrChinese;
}

/**
 * @brief CustemTextEdit::slotTextEditMaxContantNum
 * TextEdit输入允许输入最长字符串的长度
 */
void CustemTextEdit::slotTextEditMaxContantNum()
{
    QString textContent = this->toPlainText();

    int length = textContent.count();//textContent.toUtf8().size();

    if (length > m_nMaxContantLen) {
        int position = this->textCursor().position();
        QTextCursor textCursor = this->textCursor();
        textContent.remove(position - (length - m_nMaxContantLen), length - m_nMaxContantLen);
        this->setText(textContent);
        textCursor.setPosition(position - (length - m_nMaxContantLen));
        this->setTextCursor(textCursor);
        emit sigShowTips();
//        QString text = getMaxLenStr(textContent);
//        setText(m_str);
//        emit sigShowTips();
//        QTextCursor cursor = textCursor();
//        cursor.movePosition(QTextCursor::End);
//        if (cursor.hasSelection()) {
//            cursor.clearSelection();
//        }
//        //设置当前的光标为更改后的光标
//        setTextCursor(cursor);
#if 0
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::End);
        if (cursor.hasSelection()) {
            cursor.clearSelection();
        }
        cursor.deletePreviousChar();
        //设置当前的光标为更改后的光标
        setTextCursor(cursor);
#endif
    }
}
