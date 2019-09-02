#include "MainOperationWidget.h"
#include <QDebug>

MainOperationWidget::MainOperationWidget(CustomWidget *parent):
    CustomWidget (parent)
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
    createBtn(tr("缩略图"), SLOT(on_thumbnailBtn_clicked(bool)), true);
    createBtn(tr("书签"), SLOT(on_bookmarksBtn_clicked(bool)), true);
    createBtn(tr("注释"), SLOT(on_annotationBtn_clicked(bool)), true);
    m_hboxLayout->addStretch(1);
}

void MainOperationWidget::createBtn(const QString &iconName, const char *member, bool checkable, bool checked)
{
    DPushButton *btn = new DPushButton(iconName);
    btn->setToolTip(iconName);
    if (checkable) {
        btn->setCheckable(true);
        btn->setChecked(checked);

        connect(btn, SIGNAL(clicked(bool)), member);
    } else {
        connect(btn, SIGNAL(clicked()), member);
    }

    m_hboxLayout->addWidget(btn);
}

void MainOperationWidget::on_thumbnailBtn_clicked(bool)
{
    m_pMsgSubject->sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(0));
}

void MainOperationWidget::on_bookmarksBtn_clicked(bool)
{
    m_pMsgSubject->sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(1));
}

void MainOperationWidget::on_annotationBtn_clicked(bool)
{
    m_pMsgSubject->sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(2));
}

int MainOperationWidget::update(const int &, const QString &)
{
    return 0;
}
