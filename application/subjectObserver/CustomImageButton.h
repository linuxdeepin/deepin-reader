#ifndef CUSTOMIMAGEBUTTON_H
#define CUSTOMIMAGEBUTTON_H

#include <DImageButton>
#include <DWidget>
#include <QPainter>

DWIDGET_USE_NAMESPACE

class CustomImageButton : public DImageButton
{
    Q_OBJECT
public:
    CustomImageButton(DWidget *parent = nullptr);

protected:
//    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
//    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
//    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
//    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
//    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    void drawHoverPic();
    void drawPressPic();
    void drawCheckedPic();

};

#endif // CUSTOMIMAGEBUTTON_H
