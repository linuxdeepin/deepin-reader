#include "MainOperationWidget.h"
#include <QDebug>

MainOperationWidget::MainOperationWidget(CustomWidget *parent):
    CustomWidget ("MainOperationWidget", parent)
{
    initWidget();
}

void MainOperationWidget::initWidget()
{
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    hboxLayout->setSpacing(10);

    hboxLayout->addStretch(1);

    QButtonGroup *btnGroup = new QButtonGroup;  //  按钮组，　自动实现按钮唯一check属性
    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotButtonClicked(int)));

    DPushButton *btn = createBtn(tr("view"));
    btnGroup->addButton(btn, 0);
    hboxLayout->addWidget(btn);

    btn = createBtn(tr("bookmark"));
    btnGroup->addButton(btn, 1);
    hboxLayout->addWidget(btn);

    btn = createBtn(tr("comments"));
    btnGroup->addButton(btn, 2);
    hboxLayout->addWidget(btn);

    hboxLayout->addStretch(1);

    this->setLayout(hboxLayout);
}

DPushButton *MainOperationWidget::createBtn(const QString &btnName)
{
    QString normalPic = PF::getQrcPath(btnName, g_normal_state);
    QString hoverPic = PF::getQrcPath(btnName, g_hover_state);
    QString pressPic = PF::getQrcPath(btnName, g_press_state);
    QString checkedPic = PF::getQrcPath(btnName, g_checked_state);

    DPushButton *btn = new DPushButton;
    btn->setToolTip(btnName);
    btn->setCheckable(true);
    btn->setChecked(false);

    return  btn;
}

void MainOperationWidget::slotButtonClicked(int id)
{
    sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(id));
}

int MainOperationWidget::dealWithData(const int &, const QString &)
{
    return 0;
}
