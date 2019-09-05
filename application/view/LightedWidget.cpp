#include "LightedWidget.h"

LightedWidget::LightedWidget(CustomWidget *parent)
    : CustomWidget ("LightedWidget", parent)
{
    initWidget();
}

void LightedWidget::createBtn(const QString &btnName, const char *member, QHBoxLayout *bottomLayout)
{
    DPushButton *btn = new DPushButton(btnName);
    connect(btn, SIGNAL(clicked()), member);

    bottomLayout->addWidget(btn);
}

void LightedWidget::SlotBtnRedClicked()
{

}

void LightedWidget::SlotBtnGreenClicked()
{

}

void LightedWidget::SlotBtnBlueClicked()
{

}

int LightedWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void LightedWidget::initWidget()
{
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(0);
    DLabel *titleLabel = new DLabel(tr("high lighted"));
    topLayout->addWidget(titleLabel);
    topLayout->addStretch(1);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->setSpacing(0);
    createBtn(tr("red"), SLOT(SlotBtnRedClicked()), bottomLayout);
    createBtn(tr("green"), SLOT(SlotBtnGreenClicked()), bottomLayout);
    createBtn(tr("blue"), SLOT(SlotBtnBlueClicked()), bottomLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addItem(topLayout);
    mainLayout->addItem(bottomLayout);

    this->setLayout(mainLayout);
}
