#ifndef RotateImageLabel_H
#define RotateImageLabel_H

#include <DLabel>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class RotateImageLabel : public DLabel
{
    Q_OBJECT
    Q_DISABLE_COPY(RotateImageLabel)

public:
    explicit RotateImageLabel(DWidget *parent = nullptr);

public:
    void setSelect(const bool &select);

    void setRadius(const int radius);

    void setBookMarkStatus(bool bshow);

    void setBackgroundPix(QPixmap &pixmap);

    void rotateImage();

    void setHasThumbnail(const bool has) ;

    bool hasThumbnail();

    void setRotateAngle(const int &angle);

    void scaleImage(const double &);//缩略图自适应窗体大小  add by duanxiaohui 2020-3-20

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    bool m_bSelect = false;
    int m_nRadius = 8;              // 圆角
    bool m_bshowbookmark = false;   //显示书签
    QPixmap m_background;           // 缩略图
    QPixmap m_thumbPix;             // 显示缩略图
    bool m_bSetBp = false;          // 是否设置缩略图
    int m_nHighLightLineWidth = 0;  // 高亮边框宽度
    int m_nRotate = 0;              // 缩略图旋转度数
    bool m_bHasThumbnail = false;   // 加载过缩略图
    double m_scale = 1;             // 缩放比
};

#endif  // RotateImageLabel_H
