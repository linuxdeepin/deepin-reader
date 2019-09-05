#ifndef CUSTOMIMAGEBUTTON_H
#define CUSTOMIMAGEBUTTON_H

#include <DPushButton>
#include <QEvent>

DWIDGET_USE_NAMESPACE

enum BUTTON_STATE {
    NORMAL_STATE,
    NORMAL_HOVER,
    NORMAL_PRESS,
    NORMAL_CHECK
};

class CustomImageButton : public DPushButton
{
public:
    CustomImageButton(DWidget *parent = nullptr);

    void setStrNormalPic(const QString &strNormalPic);
    void setStrHoverPic(const QString &strHoverPic);
    void setStrPressPic(const QString &strPressPic);
    void setStrCheckPic(const QString &strCheckPic);

protected:
    bool event(QEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    QString m_strNormalPic = "";
    QString m_strHoverPic = "";
    QString m_strPressPic = "";
    QString m_strCheckPic = "";

private:
    int m_nBtnState = NORMAL_STATE;
};

#endif // CUSTOMIMAGEBUTTON_H
