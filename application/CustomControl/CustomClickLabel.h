#ifndef CUSTOMCLICKLABEL_H
#define CUSTOMCLICKLABEL_H

#include <DWidget>
#include <DLabel>
#include <QMouseEvent>
#include <DGuiApplicationHelper>

DWIDGET_USE_NAMESPACE

class CustomClickLabel : public DLabel
{
    Q_OBJECT
    Q_DISABLE_COPY(CustomClickLabel)

public:
    CustomClickLabel(const QString &text, DWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

public:
    void setThemePalette();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
};

#endif // CUSTOMCLICKLABEL_H
