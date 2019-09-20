#include "AppAboutWidget.h"
#include <DWidgetUtil>
#include "translator/Frame.h"

AppAboutWidget::AppAboutWidget(DWidget *parent)
    : DAboutDialog (parent)
{
    initLabels();
}

void AppAboutWidget::showScreenCenter()
{
    Dtk::Widget::moveToCenter(this);
    this->show();
    this->raise();
}

void AppAboutWidget::initLabels()
{
    setProductIcon(QPixmap(":/resources/image/logo/logo_big.svg"));
    setProductName(Frame::sAppName);
    setVersion("1.3.18");
    setDescription(Frame::sDescription);

//    setAcknowledgementLink("https://www.deepin.org/acknowledgments/deepin-image-viewer/");
    setAcknowledgementVisible(false);
}
