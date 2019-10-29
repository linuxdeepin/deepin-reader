#ifndef BOOKMARKBUTTON_H
#define BOOKMARKBUTTON_H

#include<QPushButton>
#include<QPaintEvent>
#include<QMouseEvent>

class BookMarkButton : public QPushButton
{
    Q_OBJECT
public:
    BookMarkButton(QWidget *parent = nullptr);
    bool clickState();
    void setClickState(bool state);
signals:
    void signal_bookMarkStateChange(bool);
protected:

    virtual void paintEvent(QPaintEvent *e) override;

    virtual void  enterEvent(QEvent *e) override;

    virtual void  leaveEvent(QEvent *e) override;

    virtual void  mouseEvent(QMouseEvent *e);

    virtual void mousePressEvent(QMouseEvent *e) override;

    virtual void mouseReleaseEvent(QMouseEvent *e) override;

private:
    bool  ishovered;
    bool  ispressed;
    bool isclicked;
};

#endif // BOOKMARKBUTTON_H
