#include "CustomClickLabel.h"

CustomClickLabel::CustomClickLabel(const QString &text, DWidget *parent, Qt::WindowFlags f)
    : DLabel (text, parent, f)
{

}

//  设置 主题颜色
void CustomClickLabel::setThemePalette()
{
    Dtk::Gui::DPalette plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(Dtk::Gui::DPalette::WindowText, plt.color(Dtk::Gui::DPalette::TextTips));
    setPalette(plt);
}

/**
* @brief CustomClickLabel::mousePressEvent
* 自定义label点击事件
* @param event
*/

void CustomClickLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }

    DLabel::mousePressEvent(event);
}
