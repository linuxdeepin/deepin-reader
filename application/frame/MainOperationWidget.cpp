#include "MainOperationWidget.h"
#include <QHBoxLayout>
#include <QButtonGroup>
#include "PublicFunction.h"

MainOperationWidget::MainOperationWidget(CustomWidget *parent):
    CustomWidget ("MainOperationWidget", parent)
{
    initWidget();
}

void MainOperationWidget::initWidget()
{
    auto hboxLayout = new QHBoxLayout;
    hboxLayout->setContentsMargins(2, 0, 2, 0);
    hboxLayout->setSpacing(10);

    hboxLayout->addStretch(2);

    auto btnGroup = new QButtonGroup;  //  按钮组，　自动实现按钮唯一check属性
    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotButtonClicked(int)));

    QStringList btnStrList = QStringList() << "thumbnail" << "bookmark" << "annotation";

    int nSize = btnStrList.size();
    for (int iLoop = 0; iLoop < nSize; iLoop++) {
        QString sBtn = btnStrList.at(iLoop);
        auto btn = createBtn(sBtn);
        btnGroup->addButton(btn, iLoop);
        hboxLayout->addWidget(btn);
        hboxLayout->addStretch(1);
    }

    hboxLayout->addStretch(1);

    this->setLayout(hboxLayout);

    //  缩略图 默认是 check 状态
    auto btnList = this->findChildren<DIconButton *>();
    foreach (auto btn, btnList) {
        QString objName = btn->objectName();
        if (objName == "thumbnail") {
            btn->setChecked(true);
            break;
        }
    }
}

DIconButton *MainOperationWidget::createBtn(const QString &btnName)
{
    QString normalPic = PF::getQrcPath(btnName, ImageModule::g_normal_state);

    auto btn = new DIconButton(this);
    btn->setObjectName(btnName);
    btn->setIcon(QIcon(normalPic));
    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(36, 36));
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
