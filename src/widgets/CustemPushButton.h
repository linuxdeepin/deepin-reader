#ifndef CUSTEMPUSHBUTTON_H
#define CUSTEMPUSHBUTTON_H

#include <DPushButton>
#include <QKeyEvent>

DWIDGET_USE_NAMESPACE

class CustemPushButton : public DPushButton
{
    Q_OBJECT
    Q_DISABLE_COPY(CustemPushButton)
public:
    explicit CustemPushButton(DWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // CUSTEMPUSHBUTTON_H
