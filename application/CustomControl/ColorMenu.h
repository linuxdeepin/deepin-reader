#ifndef COLORMENU_H
#define COLORMENU_H

#include <DWidget>

DWIDGET_USE_NAMESPACE

class ColorMenu : public DWidget
{
    Q_OBJECT
public:
    ColorMenu(const QColor &color, DWidget *parent = nullptr);

    inline bool isSelected()
    {
        return m_bSelected;
    }
    void setSelect(const bool select);

Q_SIGNALS:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_bSelected = false;
    QColor m_color;
};

#endif // COLORMENU_H
