#ifndef MAGNIFYLABEL_H
#define MAGNIFYLABEL_H

#include <DLabel>

DWIDGET_USE_NAMESPACE

/**
 * @brief The MagnifyLabel class
 * @brief 放大镜显示　图片
 */


class MagnifyLabel : public DLabel
{
    Q_OBJECT
public:
    MagnifyLabel(DWidget *parent = nullptr);

private slots:
    void updatePixmap(const QPixmap &);

private:
    void initBcakLabel();
};

#endif // MAGNIFYLABEL_H
