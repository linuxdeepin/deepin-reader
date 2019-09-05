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
    createBtn(tr("thumbnail"), tr(""), tr(""), tr(""), tr(""), SLOT(on_thumbnailBtn_checkedChanged(bool)));
    createBtn(tr("book mark"), tr(""), tr(""), tr(""), tr(""), SLOT(on_bookmarksBtn_checkedChanged(bool)));
    createBtn(tr("annotation"), tr(""), tr(""), tr(""), tr(""), SLOT(on_annotationBtn_checkedChanged(bool)));
    m_hboxLayout->addStretch(1);
}

void MainOperationWidget::createBtn(const QString &iconName, const QString &normalPic, const QString &hoverPic,
                                    const QString &pressPic, const QString &checkedPic, const char *member)
{
    DImageButton *btn = new DImageButton(normalPic, hoverPic, pressPic, checkedPic);
    btn->setToolTip(iconName);
    btn->setCheckable(true);
    btn->setChecked(false);

    connect(btn, SIGNAL(checkedChanged(bool)), member);

    m_hboxLayout->addWidget(btn);
}

void MainOperationWidget::on_thumbnailBtn_checkedChanged(bool)
{
    sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(0));
}

void MainOperationWidget::on_bookmarksBtn_checkedChanged(bool)
{
    sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(1));
}

void MainOperationWidget::on_annotationBtn_checkedChanged(bool)
{
    sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(2));
}

int MainOperationWidget::dealWithData(const int &, const QString &)
{
    return 0;
}
