#include "NotesItemWidget.h"
#include <DApplication>
#include <QClipboard>
#include <QTextLayout>

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
    QMenu *t_menu = new QMenu(this);
    QAction *copyAction = t_menu->addAction(tr("copy"));
    QAction *dltItemAction = t_menu->addAction(tr("Delete"));
    connect(dltItemAction, SIGNAL(triggered()), this, SLOT(slotDltNoteContant()));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(slotCopyContant()));
    t_menu->exec(QCursor::pos());
}

void NotesItemWidget::initWidget()
{
    QFont font(QString("SourceHanSansSC-Medium"),12);
    QFont fontContant(QString("SourceHanSansSC-Medium"),11);
    auto t_vLayout = new QVBoxLayout;
    t_vLayout->setContentsMargins(20, 0, 0, 0);
    t_vLayout->setSpacing(0);

    auto t_hLayout = new QHBoxLayout;
    t_hLayout->setContentsMargins(0, 0, 0, 0);
    t_hLayout->setSpacing(0);

    m_pPicture = new CustomLabel(this);
    m_pPicture->setFixedSize(QSize(40, 60));
    m_pPicture->setAlignment(Qt::AlignCenter);

    m_pPageNumber = new DLabel;
    m_pSearchResultNum = new DLabel;
    m_pTextLab = new DLabel;
    m_pTextLab->setEnabled(false);
    m_pTextLab->setFixedHeight(51);
    m_pTextLab->setMinimumWidth(102);
    m_pTextLab->setFrameStyle(QFrame::NoFrame);
    m_pTextLab->setWordWrap(true);
//    m_pTextLab->setTextBackgroundColor(this->palette().highlightedText().color());

    m_pPageNumber->setMinimumWidth(31);
    m_pPageNumber->setFixedHeight(18);
    m_pSearchResultNum->setMinimumWidth(31);
    m_pSearchResultNum->setFixedHeight(18);

    m_pPageNumber->setFont(font);
    m_pSearchResultNum->setFont(font);
    m_pTextLab->setFont(fontContant);

    t_hLayout->addWidget(m_pPageNumber);
    t_hLayout->addWidget(m_pSearchResultNum);

    t_vLayout->addItem(t_hLayout);
    t_vLayout->addWidget(m_pTextLab);

    auto m_pHLayout = new QHBoxLayout;

    m_pHLayout->setSpacing(1);
    m_pHLayout->setContentsMargins(1, 0, 1, 0);
    m_pHLayout->addWidget(m_pPicture);
    m_pHLayout->addItem(t_vLayout);
    m_pHLayout->setSpacing(1);

    this->setLayout(m_pHLayout);
}

int NotesItemWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void NotesItemWidget::paintEvent(QPaintEvent *e)
{
    if(m_pTextLab){
        QString note = m_strNote;
        m_pTextLab->setText(calcText(m_pTextLab->font(), note, m_pTextLab->size()));
    }

    CustomItemWidget::paintEvent(e);

    QPalette p(m_pPicture->palette());

    //  涉及到 主题颜色
    if (m_bPaint) {
        p.setColor(QPalette::Text, Qt::blue);
    } else {
        p.setColor(QPalette::Text, Qt::black);
    }

    m_pPicture->setPalette(p);

    update();
}

QString NotesItemWidget::calcText(const QFont &font, const QString &note, const QSize &size/*const int MaxWidth*/)
{
#if 0
    QString text = note;

    QFontMetrics fontWidth(font);
        int width = fontWidth.width(note);  //计算字符串宽度
        if(width>=size.width())  //当字符串宽度大于最大宽度时进行转换
        {
            text = fontWidth.elidedText(text,Qt::ElideRight,size.width());  //右部显示省略号
        }
        return text;   //返回处理后的字符串
#endif

#if 1
        QString text;
        QString tempText;
        int totalHeight = size.height();
        int lineWidth = size.width()-12;

        QFontMetrics fm(font);

        int calcHeight = 0;
        int lineHeight = fm.height();
        int lineSpace = 0;
        int lineCount = (totalHeight - lineSpace) / lineHeight;
        int prevLineCharIndex = 0;
        for(int charIndex=0; charIndex<note.size() && lineCount >=0; ++charIndex)
        {
            int fmWidth = fm.horizontalAdvance(tempText);
            if(fmWidth > lineWidth)
            {
                calcHeight += lineHeight/*+3*/;
                if (calcHeight + lineHeight > totalHeight) {
                    QString endString = note.mid(prevLineCharIndex);
                    const QString &endText = fm.elidedText(endString, Qt::ElideRight, size.width());
                    text += endText;

                    lineCount = 0;
                    break;
                }

                QChar currChar = tempText.at(tempText.length()-1);
                QChar nextChar = note.at(note.indexOf(tempText)+tempText.length());
                if(currChar.isLetter() && nextChar.isLetter())
                {
                    tempText += '-';
                }
                fmWidth = fm.horizontalAdvance(tempText);
                if (fmWidth > size.width()) {
                    --charIndex;
                    --prevLineCharIndex;
                    tempText = tempText.remove(tempText.length()-2, 1);
                }
                text += tempText;

                --lineCount;
                if(lineCount > 0)
                {
                    text += "\n";
                }
                tempText = note.at(charIndex);

                prevLineCharIndex = charIndex;
            }
            else
            {
                tempText += note.at(charIndex);
            }
        }

        if (lineCount > 0)
        {
            text += tempText;
        }

        return text;
#endif
}
