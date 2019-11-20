#include "NoteTipWidget.h"
#include "CustomControl/CustomClickLabel.h"
#include <QHBoxLayout>
#include <DTextEdit>

NoteTipWidget::NoteTipWidget(CustomWidget *parnet)
    : CustomWidget ("NoteTipWidget", parnet)
{
    setWindowFlags(Qt::ToolTip);
    setFixedSize(100, 100);

    initWidget();
    initConnection();
    slotUpdateTheme();
}

void NoteTipWidget::setTipContent(const QString &content)
{
    auto label = this->findChild<DTextEdit *>();
    if (label) {
        label->setText(content);
    }
}

void NoteTipWidget::initConnection()
{
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
}

void NoteTipWidget::slotUpdateTheme()
{
    updateWidgetTheme();
}

int NoteTipWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }
    return 0;
}

void NoteTipWidget::initWidget()
{
    auto label = new DTextEdit(this);
    label->setFrameShape(QFrame::NoFrame);
    label->setReadOnly(true);

    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);

    this->setLayout(layout);
}
