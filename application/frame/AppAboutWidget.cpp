#include "AppAboutWidget.h"
#include <DWidgetUtil>
#include "subjectObserver/ModuleHeader.h"

AppAboutWidget::AppAboutWidget(DWidget *parent)
    : DAboutDialog (parent)
{
    initLabels();
}

void AppAboutWidget::initLabels()
{
    setProductIcon(QPixmap(":/resources/image/logo/logo_big.svg"));
    setProductName(tr("deepin-reader"));
    setVersion("20191018");

//    setDescription(Frame::sDescription);

    setAcknowledgementLink(Constant::sAcknowledgementLink);
    setAcknowledgementVisible(false);
}
