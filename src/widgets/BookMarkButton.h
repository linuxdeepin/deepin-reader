#ifndef BOOKMARKBUTTON_H
#define BOOKMARKBUTTON_H

#include <DPushButton>
#include <DWidget>

class BookMarkButton : public Dtk::Widget::DPushButton
{
    Q_OBJECT
public:
    BookMarkButton(Dtk::Widget::DWidget *parent = nullptr);

    bool clickState();

    void setClickState(bool state);

signals:
    void sigClicked(bool);

protected:

    virtual void paintEvent(QPaintEvent *e) override;

    virtual void  enterEvent(QEvent *e) override;

    virtual void  leaveEvent(QEvent *e) override;

    virtual void mousePressEvent(QMouseEvent *e) override;

    virtual void mouseReleaseEvent(QMouseEvent *e) override;

private:
    bool  ishovered;
    bool  ispressed;
    bool  isclicked;
};

#endif // BOOKMARKBUTTON_H
