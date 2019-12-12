#ifndef CUSTEMICONBUTTON_H
#define CUSTEMICONBUTTON_H

#include <DIconButton>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

class CustemIconButton : public DIconButton
{
    Q_OBJECT
    Q_DISABLE_COPY(CustemIconButton)
public:
    explicit CustemIconButton(QWidget *parent = nullptr);
    ~CustemIconButton() Q_DECL_OVERRIDE;

    void setStatus(bool checked);

protected:
    void enterEvent(QEvent *) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
};

#endif  // CUSTEMICONBUTTON_H
