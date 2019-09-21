#include "MainOperationWidget.h"
//#include <DIconButton>
#include "translator/Frame.h"

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

    QStringList btnList = QStringList() << Frame::sThumbnail << Frame::sBookmark << Frame::sAnnotation;

    int nSize = btnList.size();
    for (int iLoop = 0; iLoop < nSize; iLoop++) {
        QString sBtn = btnList.at(iLoop);
        DPushButton *btn = createBtn(sBtn);
        btnGroup->addButton(btn, iLoop);
        hboxLayout->addWidget(btn);
    }

    hboxLayout->addStretch(1);

    this->setLayout(hboxLayout);
}

DPushButton *MainOperationWidget::createBtn(const QString &btnName)
{
    QString normalPic = PF::getQrcPath(btnName, ImageModule::g_normal_state);
    QString hoverPic = PF::getQrcPath(btnName, ImageModule::g_hover_state);
    QString pressPic = PF::getQrcPath(btnName, ImageModule::g_press_state);
    QString checkedPic = PF::getQrcPath(btnName, ImageModule::g_checked_state);

    //DIconButton *btn1 = new DIconButton;
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
