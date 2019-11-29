#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H
#include <DTextEdit>
DWIDGET_USE_NAMESPACE

class CustomTextEdit: DTextEdit
{
public:
    CustomTextEdit(DWidget *parent = nullptr);
    ~CustomTextEdit();
    void setContent(const QString &content, int iline, int iwidth, Qt::TextElideMode mode = Qt::ElideRight);
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // CUSTOMTEXTEDIT_H
