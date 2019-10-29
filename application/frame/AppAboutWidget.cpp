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
    setProductIcon(QPixmap(Constant::sLogoPath));
    setProductName(tr("deepin-reader"));
    setVersion("20191018");

    setDescription(tr("Document viewer is a document viewer that comes with the deep operating system.\r\n In"
                      " addition to opening and reading PDF files, On documents you can also \r\n add"
                      " bookmark, annotation and highlight selected text."));

    setAcknowledgementLink(Constant::sAcknowledgementLink);
    setAcknowledgementVisible(false);
}
