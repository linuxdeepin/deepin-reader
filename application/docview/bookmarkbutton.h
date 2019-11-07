#ifndef BOOKMARKBUTTON_H
#define BOOKMARKBUTTON_H

#include<QPaintEvent>
#include<QMouseEvent>
#include<DPushButton>
DWIDGET_USE_NAMESPACE

class BookMarkButton : public DPushButton
{
    Q_OBJECT
public:
    BookMarkButton(DWidget *parent = nullptr);
    bool clickState();
    void setClickState(bool state);
signals:
    void signal_bookMarkStateChange(bool);
protected:

    virtual void paintEvent(QPaintEvent *e) override;

    virtual void  enterEvent(QEvent *e) override;

    virtual void  leaveEvent(QEvent *e) override;

//    virtual void  mouseEvent(QMouseEvent *e);

    virtual void mousePressEvent(QMouseEvent *e) override;

    virtual void mouseReleaseEvent(QMouseEvent *e) override;

private:
    bool  ishovered;
    bool  ispressed;
    bool isclicked;
};

#endif // BOOKMARKBUTTON_H
