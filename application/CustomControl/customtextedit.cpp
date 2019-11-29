#include "customtextedit.h"

CustomTextEdit::CustomTextEdit(DWidget *parent): DTextEdit(parent)
{
    setFrameShape(QFrame::NoFrame);
    setReadOnly(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWordWrapMode(QTextOption::WrapAnywhere /*WordWrap*/);

    setAttribute(Qt::WA_TranslucentBackground);
}

CustomTextEdit::~CustomTextEdit()
{

}

void CustomTextEdit::setContent(const QString &content, int iline, int iwidth, Qt::TextElideMode mode)
{
    QFontMetrics fm(font());
    int pixelsWide = fm.horizontalAdvance(content);
    int pixelsHigh = fm.height();
    QString strcontent;

    if (pixelsWide > iwidth * iline) {
        setFixedHeight(pixelsHigh * iline + iline);
        strcontent = fontMetrics().elidedText(content, mode, (iwidth - 10) * iline, Qt::TextWordWrap);
    } else {
        setFixedHeight(pixelsHigh * iline + iline);
        strcontent = content;
    }
    setFixedWidth(iwidth);
    setText(strcontent);

}

void CustomTextEdit::paintEvent(QPaintEvent *event)
{
    DTextEdit::paintEvent(event);
}


