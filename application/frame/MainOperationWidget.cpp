#include "MainOperationWidget.h"
#include <DToolButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include "controller/AppSetting.h"
#include "controller/DataManager.h"
#include "frame/DocummentFileHelper.h"
#include "utils/PublicFunction.h"

MainOperationWidget::MainOperationWidget(CustomWidget *parent)
    : CustomWidget("MainOperationWidget", parent)
{
    initWidget();
    initConnect();
    slotUpdateTheme();
}

/**
 * @brief MainOperationWidget::setOperatAction
 * 打开文档，展示上次按钮状态
 * @param index
 */
void MainOperationWidget::setOperatAction(const int &index)
{
    auto btnBox = this->findChild<QButtonGroup *>();
    if (btnBox) {
        auto btn = btnBox->button(index);
        if (btn) {
            btn->setChecked(true);
        }
    }
}

void MainOperationWidget::initWidget()
{
    auto hboxLayout = new QHBoxLayout;
    hboxLayout->setContentsMargins(19, 0, 19, 0);

    hboxLayout->addStretch();

    auto btnGroup = new QButtonGroup(this);  //  按钮组，　自动实现按钮唯一check属性
    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotButtonClicked(int)));

    QStringList btnStrList = QStringList() << tr("Thumbnails") << tr("Bookmarks") << tr("Annotations");
    QStringList btnObjList = QStringList() << "thumbnail" << "bookmark" << "annotation";

    int nSize = btnStrList.size();
    for (int iLoop = 0; iLoop < nSize; iLoop++) {
        QString sBtn = btnStrList.at(iLoop);
        QString sObj = btnObjList.at(iLoop);
        auto btn = createBtn(sBtn, sObj);
        btnGroup->addButton(btn, iLoop);
        hboxLayout->addWidget(btn);
        hboxLayout->addSpacing(40);
    }

    auto pHideBtn = new DPushButton(this);
    pHideBtn->setObjectName("hideBtn");
    pHideBtn->setVisible(false);
    pHideBtn->setCheckable(true);
    btnGroup->addButton(pHideBtn);
    hboxLayout->addWidget(pHideBtn);

    hboxLayout->addStretch();

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

DToolButton *MainOperationWidget::createBtn(const QString &btnName, const QString &objName)
{
    // auto btn = new DIconButton(this);
    auto btn = new DToolButton(this);
    btn->setToolTip(btnName);
    btn->setObjectName(objName);
    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(36, 36));
    btn->setCheckable(true);
    btn->setChecked(false);

    return btn;
}

void MainOperationWidget::initConnect()
{
    connect(this, SIGNAL(sigSearchControl()), SLOT(slotSearchControl()));
    connect(this, SIGNAL(sigSearchClosed()), SLOT(slotSearchClosed()));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
}

//  主题更新
void MainOperationWidget::slotUpdateTheme()
{
    updateWidgetTheme();

    QIcon icon;
    auto btnList = this->findChildren<DToolButton *>();
    foreach (auto btn, btnList) {
        QString objName = btn->objectName();
        if (objName != "") {
            icon = PF::getIcon(Pri::g_module + objName);
            btn->setIcon(icon);
        }
    }
}

void MainOperationWidget::slotButtonClicked(int id)
{
    emit sigShowStackWidget(id);
}

/**
 * @brief MainOperationWidget::slotSearchControl
 * 搜索内容时，此模块不可用
 */
void MainOperationWidget::slotSearchControl()
{
    auto hideBtn = this->findChild<DPushButton *>("hideBtn");
    if (hideBtn) {
        hideBtn->setChecked(true);
    }
}

/**
 * @brief MainOperationWidget::slotSearchClosed
 * 搜索结束后，回复结束后的状态
 */
void MainOperationWidget::slotSearchClosed()
{
    int indexWidget = 0;

    indexWidget = AppSetting::instance()->getKeyValue(KEY_WIDGET).toInt();

    emit sigShowStackWidget(indexWidget);

    setOperatAction(indexWidget);
}

int MainOperationWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_FIND_CONTENT) {  //  查询内容
        emit sigSearchControl();
    } else if (msgType == MSG_CLEAR_FIND_CONTENT) {
        emit sigSearchClosed();
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }

    return 0;
}
