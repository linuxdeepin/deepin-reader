#include "MainOperationWidget.h"
#include <QHBoxLayout>
#include <QButtonGroup>
#include "PublicFunction.h"

MainOperationWidget::MainOperationWidget(CustomWidget *parent):
    CustomWidget ("MainOperationWidget", parent)
{
    initWidget();
    initConnect();
}

void MainOperationWidget::initWidget()
{
    auto hboxLayout = new QHBoxLayout;
    hboxLayout->setContentsMargins(2, 0, 2, 0);
    hboxLayout->setSpacing(10);

    hboxLayout->addStretch(1);

    auto btnGroup = new QButtonGroup;  //  按钮组，　自动实现按钮唯一check属性
    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotButtonClicked(int)));

    QStringList btnStrList = QStringList() << "thumbnail" << "bookmark" << "annotation";

    int nSize = btnStrList.size();
    for (int iLoop = 0; iLoop < nSize; iLoop++) {
        QString sBtn = btnStrList.at(iLoop);
        auto btn = createBtn(sBtn);
        btnGroup->addButton(btn, iLoop);
        hboxLayout->addWidget(btn);
    }

    hboxLayout->addStretch(1);

    this->setLayout(hboxLayout);

    //  缩略图 默认是 check 状态
    auto btnList = this->findChildren<DIconButton *>();
    foreach (auto btn, btnList) {
        QString objName = btn->objectName();
        if (objName == "thumbnail") {
            btn->setChecked(true);
            m_nThumbnailIndex = 0;
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

QString MainOperationWidget::findBtnName()
{
    QString btnName;

    if (0 == m_nThumbnailIndex) {
        btnName = "thumbnail";
    } else if (1 == m_nThumbnailIndex) {
        btnName = "bookmark";
    } else if (2 == m_nThumbnailIndex) {
        btnName = "annotation";
    }

    return btnName;
}

void MainOperationWidget::initConnect()
{
    connect(this, SIGNAL(sigSearchControl()), this, SLOT(slotSearchControl()));
    connect(this, SIGNAL(sigSearchClosed()), this, SLOT(slotSearchClosed()));
}

void MainOperationWidget::slotButtonClicked(int id)
{
    sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(id));
    m_nThumbnailIndex = id;
}

/**
 * @brief MainOperationWidget::slotSearchControl
 * 搜索内容时，此模块不可用
 */
void MainOperationWidget::slotSearchControl()
{
    auto btnList = this->findChildren<DIconButton *>();
    foreach (auto btn, btnList) {
        btn->setChecked(false);
        btn->setEnabled(false);
    }
}

/**
 * @brief MainOperationWidget::slotSearchClosed
 * 搜索结束后，回复结束后的状态
 */
void MainOperationWidget::slotSearchClosed()
{
    auto btnList = this->findChildren<DIconButton *>();
    foreach (auto btn, btnList) {
        QString objName = btn->objectName();
        if (objName == findBtnName()) {
            btn->setChecked(true);
            sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(m_nThumbnailIndex));
        }
        btn->setEnabled(true);
    }
}

int MainOperationWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_FIND_CONTENT) {        //  查询内容
        emit sigSearchControl();
    }

    if (msgType == MSG_CLEAR_FIND_CONTENT) {
        emit sigSearchClosed();
    }
    return 0;
}
