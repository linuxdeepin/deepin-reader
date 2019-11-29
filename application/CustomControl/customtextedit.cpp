#include "customtextedit.h"

CustomTextEdit::CustomTextEdit(DWidget *parent): DTextEdit(parent)
{
    setFrameShape(QFrame::NoFrame);
    setReadOnly(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWordWrapMode(QTextOption::WrapAnywhere /*WordWrap*/);
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
        strcontent = fontMetrics().elidedText(content, mode, (iwidth - 16) * 10, Qt::TextWordWrap);
    } else {
        setFixedHeight(pixelsHigh * iline + iline);
        strcontent = content;
    }
    setFixedWidth(iwidth);
    setText(strcontent);

}


