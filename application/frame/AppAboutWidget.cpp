#include "AppAboutWidget.h"
#include <DWidgetUtil>

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
    setProductName(tr("deepin_reader"));
    setVersion("1.3.18");
    setDescription(tr("文档查看器是深度操作系统自带的文档查看软件。\r\n"
                      "除了打开和阅读PDF文件外，你还可以对文档进行\r\n"
                      "添加书签、添加注释和对选择的文本进行高亮显示等操作。"));

    setAcknowledgementVisible(false);
}
