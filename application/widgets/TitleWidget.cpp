#include "TitleWidget.h"
#include <QDebug>

TitleWidget::TitleWidget(DWidget *parent) :
    DWidget(parent)
{
    setFixedWidth(200);

    m_layout = new QHBoxLayout();
    setLayout(m_layout);

    initBtns();
}

//  缩略图 显示
void TitleWidget::on_thumbnailBtn_clicked(bool bCheck)
{
    emit sendThumbnailState(bCheck);
}

//  字体
void TitleWidget::on_fontBtn_clicked()
{

}

//  放大镜 功能
void TitleWidget::on_magnifyingBtn_clicked(bool bCheck)
{

}

//  初始化 标题栏 按钮
void TitleWidget::initBtns()
{
    createBtn(tr("缩略图"), SLOT(on_thumbnailBtn_clicked(bool)), true);
    createBtn(tr("字体"), SLOT(on_fontBtn_clicked()));
    DComboBox* comboBox = new  DComboBox;
    comboBox->addItem(tr("123"));
    m_layout->addWidget(comboBox);
    createBtn(tr("放大镜"), SLOT(on_magnifyingBtn_clicked(bool)), true);
}

void TitleWidget::createBtn(const QString& iconName, const char* member, bool checkable, bool checked)
{
    DPushButton * btn = new DPushButton(iconName);

    if(checkable)
    {
        btn->setCheckable(true);
        btn->setChecked(checked);

        connect(btn, SIGNAL(clicked(bool)), member);
    }
    else
    {
        connect(btn, SIGNAL(clicked()), member);
    }

    m_layout->addWidget(btn);
}
