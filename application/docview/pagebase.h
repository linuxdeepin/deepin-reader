#ifndef PAGEBASE_H
#define PAGEBASE_H
#include <QObject>
#include <QLabel>

enum RotateType_EM {
    RotateType_Normal = 0,
    RotateType_90,
    RotateType_180,
    RotateType_270
};

class PageBase: public QLabel
{
    Q_OBJECT
public:
    PageBase(QWidget *parent = 0);
protected:
    RotateType_EM m_rotate;
    double m_scale;
};

#endif // PAGEBASE_H
