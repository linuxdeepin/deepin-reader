#ifndef IMAGEFRAME_H
#define IMAGEFRAME_H

#include <DFrame>

DWIDGET_USE_NAMESPACE

class ImageFrame : public DFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageFrame)
public:
    explicit ImageFrame(QWidget* parent = nullptr);

    void setBackgroundPix(QPixmap pixmap);

protected:
    void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;

private:
    QPixmap m_background;  // 缩略图
};

#endif  // IMAGEFRAME_H
