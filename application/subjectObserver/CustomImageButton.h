#ifndef CUSTOMIMAGEBUTTON_H
#define CUSTOMIMAGEBUTTON_H

#include <DImageButton>
#include <DWidget>
#include <QPainter>

DWIDGET_USE_NAMESPACE

class CustomImageButton : public DImageButton
{
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
    void drawNorPic();
    void drawHoverPic();
    void drawPressPic();

};

#endif // CUSTOMIMAGEBUTTON_H
