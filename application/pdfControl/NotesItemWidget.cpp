#include "NotesItemWidget.h"
#include <DApplication>
#include <DFontSizeManager>
#include <QClipboard>
#include <QTextLayout>
#include "utils/utils.h"

NotesItemWidget::NotesItemWidget(CustomItemWidget *parent) :
    CustomItemWidget(QString("NotesItemWidget"), parent)
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(slotShowContextMenu(const QPoint &)));

    initWidget();
}

void NotesItemWidget::setTextEditText(const QString &contant)
{
    m_strNote = contant;
    if (m_pTextLab) {
        m_pTextLab->clear();
        m_pTextLab->setText(contant);
    }
}

void NotesItemWidget::setSerchResultText(const QString &result)
{
    if (m_pSearchResultNum) {
        m_pSearchResultNum->setText(result);
    }
}

void NotesItemWidget::setBSelect(const bool &paint)
{
    if (m_pPicture) {
        m_pPicture->setSelect(paint);
    }
    m_bPaint = paint;
    update();
}

void NotesItemWidget::slotDltNoteContant()
{
    sendMsg(MSG_NOTE_DLTNOTEITEM, m_strUUid);
}

void NotesItemWidget::slotCopyContant()
{
    if (m_pTextLab) {
        QString str = m_pTextLab->text();
        if (str != QString("")) {
            QClipboard *clipboard = DApplication::clipboard();   //获取系统剪贴板指针
            clipboard->setText(str);
        }
    }
}

void NotesItemWidget::slotShowContextMenu(const QPoint &)
{
    if (!m_isNote) {
        return;
    }
    if(m_menu == nullptr){
        m_menu = new DMenu(this);
        QAction *copyAction = m_menu->addAction(tr("copy"));
        m_menu->addSeparator();
        QAction *dltItemAction = m_menu->addAction(tr("Delete"));
        connect(dltItemAction, SIGNAL(triggered()), this, SLOT(slotDltNoteContant()));
        connect(copyAction, SIGNAL(triggered()), this, SLOT(slotCopyContant()));
    }

    if(m_menu){
        m_menu->exec(QCursor::pos());
    }
}

void NotesItemWidget::initWidget()
{

    m_pPicture = new ImageLabel(this);
    m_pPicture->setFixedSize(QSize(48, 68));
    m_pPicture->setAlignment(Qt::AlignCenter);

//    QFont font;
//    font = Utils::getPixFont(QString("SourceHanSansSC-Medium"), 12);

    m_pPageNumber = new PageNumberLabel;
    m_pPageNumber->setEnabled(false);
    m_pPageNumber->setMinimumWidth(31);
    m_pPageNumber->setFixedHeight(18);
//    m_pPageNumber->setFont(font);
    DFontSizeManager::instance()->bind(m_pPageNumber, DFontSizeManager::T8);

    m_pSearchResultNum = new DLabel;
    m_pSearchResultNum->setEnabled(false);
    m_pSearchResultNum->setMinimumWidth(31);
    m_pSearchResultNum->setFixedHeight(18);
//    m_pSearchResultNum->setFont(font);
    DFontSizeManager::instance()->bind(m_pSearchResultNum, DFontSizeManager::T8);

//    font = Utils::getPixFont(QString("SourceHanSansSC-Medium"), 11);
    m_pTextLab = new DLabel;
    m_pTextLab->setFixedHeight(51);
    m_pTextLab->setMinimumWidth(102);
    m_pTextLab->setMaximumWidth(349);
    m_pTextLab->setFrameStyle(QFrame::NoFrame);
    m_pTextLab->setWordWrap(true);
//    m_pTextLab->setFont(font);
    DFontSizeManager::instance()->bind(m_pTextLab, DFontSizeManager::T9);

    auto hLine = new DHorizontalLine;

    auto t_hLayout = new QHBoxLayout;
    t_hLayout->setContentsMargins(0, 0, 0, 0);
    t_hLayout->setSpacing(0);
    t_hLayout->addWidget(m_pPageNumber);
    t_hLayout->addWidget(m_pSearchResultNum);

    auto t_vLayout = new QVBoxLayout;
    t_vLayout->setContentsMargins(20, 0, 0, 0);
    t_vLayout->setSpacing(0);
    t_vLayout->addItem(t_hLayout);
    t_vLayout->addWidget(m_pTextLab);
    t_vLayout->addWidget(hLine);

    auto m_pHLayout = new QHBoxLayout;

    m_pHLayout->setSpacing(1);
    m_pHLayout->setContentsMargins(1, 0, 1, 0);
    m_pHLayout->addWidget(m_pPicture);
    m_pHLayout->addItem(t_vLayout);
    m_pHLayout->setSpacing(1);

    this->setLayout(m_pHLayout);
}

int NotesItemWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_NOTIFY_KEY_MSG == msgType) {
        if (msgContent == KeyStr::g_del) {
            bool bFocus = this->hasFocus();
            if (bFocus) {
                slotDltNoteContant();
            }
        }
    }
    return 0;
}

void NotesItemWidget::paintEvent(QPaintEvent *e)
{
    if (m_pTextLab) {
        QString note = m_strNote;
        m_pTextLab->setText(calcText(m_pTextLab->font(), note, m_pTextLab->size()));
    }

    CustomItemWidget::paintEvent(e);

    QPalette text(m_pTextLab->palette());
    text.setColor(QPalette::Text, QColor(QString("#303030")));
    m_pTextLab->setPalette(text);
    QPalette page(m_pPageNumber->palette());
    page.setColor(QPalette::Text, QColor(QString("#7587A7")));
    m_pPageNumber->setPalette(page);
    QPalette reslt(m_pSearchResultNum->palette());
    reslt.setColor(QPalette::Text, QColor(QString("#7587A7")));
    m_pSearchResultNum->setPalette(reslt);

    QPalette p(m_pPicture->palette());
    QColor color;

    //  涉及到 主题颜色
    if (m_bPaint) {
        color = QColor(QString("#0081FF"));
        p.setColor(QPalette::Text, p.highlight().color());
    } else {
        color = QColor::fromRgbF(0, 0, 0, 0.08);
        p.setColor(QPalette::Text, /*p.shadow().color()*/color);
    }

    m_pPicture->setPalette(p);

//    int width = this->width() - 10;
//    int height = this->height() - 1;

//    int sPoint = width - m_pTextLab->width() + 5;

//    QPainter painter(this);
//    painter.setPen(QPen(QColor(QString("#D8D8D8")), 1));

//    QPoint startP(sPoint, height);
//    QPoint endP(width, height);

//    painter.drawLine(startP, endP);
}

QString NotesItemWidget::calcText(const QFont &font, const QString &note, const QSize &size/*const int MaxWidth*/)
{
#if 0
    QString text = note;

    QFontMetrics fontWidth(font);
    int width = fontWidth.width(note);  //计算字符串宽度
    if (width >= size.width()) { //当字符串宽度大于最大宽度时进行转换
        text = fontWidth.elidedText(text, Qt::ElideRight, size.width()); //右部显示省略号
    }
    return text;   //返回处理后的字符串
#endif

#if 1
    QString text;
    QString tempText;
    int totalHeight = size.height();
    int lineWidth = size.width() - 12;

    QFontMetrics fm(font);

    int calcHeight = 0;
    int lineHeight = fm.height();
    int lineSpace = 0;
    int lineCount = (totalHeight - lineSpace) / lineHeight;
    int prevLineCharIndex = 0;
    for (int charIndex = 0; charIndex < note.size() && lineCount >= 0; ++charIndex) {
        int fmWidth = fm.horizontalAdvance(tempText);
        if (fmWidth > lineWidth) {
            calcHeight += lineHeight/*+3*/;
            if (calcHeight + lineHeight > totalHeight) {
                QString endString = note.mid(prevLineCharIndex);
                const QString &endText = fm.elidedText(endString, Qt::ElideRight, size.width());
                text += endText;

                lineCount = 0;
                break;
            }

            QChar currChar = tempText.at(tempText.length() - 1);
            QChar nextChar = note.at(note.indexOf(tempText) + tempText.length());
            if (currChar.isLetter() && nextChar.isLetter()) {
                tempText += '-';
            }
            fmWidth = fm.horizontalAdvance(tempText);
            if (fmWidth > size.width()) {
                --charIndex;
                --prevLineCharIndex;
                tempText = tempText.remove(tempText.length() - 2, 1);
            }
            text += tempText;

            --lineCount;
            if (lineCount > 0) {
                text += "\n";
            }
            tempText = note.at(charIndex);

            prevLineCharIndex = charIndex;
        } else {
            tempText += note.at(charIndex);
        }
    }

    if (lineCount > 0) {
        text += tempText;
    }

    return text;
#endif
}
