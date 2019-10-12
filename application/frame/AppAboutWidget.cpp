#include "AppAboutWidget.h"
#include <DWidgetUtil>
#include "subjectObserver/ModuleHeader.h"
#include "translator/Frame.h"

AppAboutWidget::AppAboutWidget(DWidget *parent)
    : DAboutDialog (parent)
{
    initLabels();
}

void AppAboutWidget::initLabels()
{
    setProductIcon(QPixmap(":/resources/image/logo/logo_big.svg"));
    setProductName(Frame::sAppName);
    setVersion("20191011");
    setDescription(Frame::sDescription);

    setAcknowledgementLink(Constant::sAcknowledgementLink);
    setAcknowledgementVisible(false);
}
