#include "MainOperationWidget.h"
#include <QDebug>

MainOperationWidget::MainOperationWidget(CustomWidget *parent):
    CustomWidget ("MainOperationWidget", parent)
{
    m_hboxLayout = new QHBoxLayout;
    m_hboxLayout->setContentsMargins(0, 0, 0, 0);
    m_hboxLayout->setSpacing(0);

    this->setLayout(m_hboxLayout);

    initBtns();
}

void MainOperationWidget::initWidget()
{

}

void MainOperationWidget::initBtns()
{
    m_hboxLayout->addStretch(1);
    createBtn(tr("view"),  SLOT(on_thumbnailBtn_checkedChanged()));
    createBtn(tr("bookmark"), SLOT(on_bookmarksBtn_checkedChanged()));
    createBtn(tr("comments"),  SLOT(on_annotationBtn_checkedChanged()));
    m_hboxLayout->addStretch(1);
}

void MainOperationWidget::createBtn(const QString &btnName, const char *member)
{
    QString normalPic = Utils::getQrcPath(btnName, g_normal_state);
    QString hoverPic = Utils::getQrcPath(btnName, g_hover_state);
    QString pressPic = Utils::getQrcPath(btnName, g_press_state);
    QString checkedPic = Utils::getQrcPath(btnName, g_checked_state);

    DImageButton *btn = new DImageButton(normalPic, hoverPic, pressPic, checkedPic);
    btn->setToolTip(btnName);
    btn->setCheckable(true);
    btn->setChecked(false);

    connect(btn, SIGNAL(clicked()), member);

    m_hboxLayout->addWidget(btn);
}

void MainOperationWidget::on_thumbnailBtn_checkedChanged()
{
    sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(0));
}

void MainOperationWidget::on_bookmarksBtn_checkedChanged()
{
    sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(1));
}

void MainOperationWidget::on_annotationBtn_checkedChanged()
{
    sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(2));
}

int MainOperationWidget::dealWithData(const int &, const QString &)
{
    return 0;
}
