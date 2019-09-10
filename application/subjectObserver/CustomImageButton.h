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
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    void initThemeChanged();

private:
    void drawHoverPic();
    void drawPressPic();
    void drawCheckedPic();

private:
    QColor      m_pHoverColr;
    QColor      m_pPressColor;
    QColor      m_pCheckColor;
};

#endif // CUSTOMIMAGEBUTTON_H
