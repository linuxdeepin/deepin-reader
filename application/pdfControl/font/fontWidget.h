#ifndef FONTWIDGET_H
#define FONTWIDGET_H

#include <DLabel>
#include <DSlider>

#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QPixmap>
#include <QPainter>
#include <QRectF>
#include <QtMath>

#include "subjectObserver/CustomWidget.h"
#include "docview/docummentproxy.h"

/**
 * @brief The MenuLab class
 * 自定义label，提供点击事件
 */

class MenuLab : public DLabel
{
    Q_OBJECT
public:
    MenuLab(QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
};

/**
 * @brief The FontWidget class
 * 字体调整窗体
 */
class FontWidget : public CustomWidget
{
    Q_OBJECT
public:
    FontWidget(CustomWidget *parent = nullptr);

signals:
    void sigUpdateTheme(const QString &);
    void sigWidgetHide();
    void sigOpenFileOk();

public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

protected:
    void initWidget() Q_DECL_OVERRIDE;

private slots:
    void slotUpdateTheme(const QString &);
    void slotReset();

private:
    void rotateFileView(bool isRight = true);
    void scaleAndRotate(int);
    void setShowSuitHIcon();
    void setShowSuitWIcon();
    void initConnection();

    DLabel *getLabelLineH();

    void initScaleLabel();
    void initScaleSlider();
    void initDowbleShow();
    void initAdaptateHeight();
    void initAdaptateWidght();

private slots:
    void slotSetChangeVal(int);
    void slotSetDoubPageViewCheckIcon();
    void slotSetSuitHCheckIcon();
    void slotSetSuitWCheckIcon();
    void slotSetRotateLeftCheckIcon();
    void slotSetRotateRightCheckIcon();

private:
    QHBoxLayout     *m_pDoubleShowLayout = nullptr;
    QHBoxLayout     *m_pAdaptateHeightLayout = nullptr;
    QHBoxLayout     *m_pAdaptateWidghtLayout = nullptr;

    DLabel *m_pEnlargeLab = nullptr;         // 缩放比例
    DSlider *m_pEnlargeSlider = nullptr;     // 缩放比例slider

    MenuLab *m_pDoubPageViewLab = nullptr;    // 双页显示ICON标签
    MenuLab *m_pSuitHLab = nullptr;           // 自适应高度ICON标签
    MenuLab *m_pSuitWLab = nullptr;           // 自适应宽度ICON标签
    int m_rotate = 0;                        // 旋转角度
    int m_rotateType = RotateType_Normal;    // 旋转类型
    bool m_bSuitH = false;                   // 自适应高度
    bool m_bSuitW = false;                   // 自适应宽度
    bool m_isDoubPage = false;                  //  双页显示标志
    bool m_bIsAdaptMove = false;
};

#endif // FONTWIDGET_H
