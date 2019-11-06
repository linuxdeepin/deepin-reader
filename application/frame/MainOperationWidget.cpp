#include "MainOperationWidget.h"
#include <QHBoxLayout>
#include <QButtonGroup>
#include "utils/PublicFunction.h"
#include <DGuiApplicationHelper>

MainOperationWidget::MainOperationWidget(CustomWidget *parent):
    CustomWidget ("MainOperationWidget", parent)
{
    initWidget();
    initConnect();
    slotUpdateTheme();
}

void MainOperationWidget::initWidget()
{
    DPalette plt=DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(QPalette::Background, plt.color(QPalette::Base));
    setAutoFillBackground(true);
    setPalette(plt);
    auto hboxLayout = new QHBoxLayout;
    hboxLayout->setContentsMargins(2, 0, 2, 0);
    hboxLayout->setSpacing(10);

    hboxLayout->addStretch(1);

    auto btnGroup = new QButtonGroup(this);  //  按钮组，　自动实现按钮唯一check属性
    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotButtonClicked(int)));

    QStringList btnStrList = QStringList() << tr("thumbnail") << tr("bookmark") << tr("annotation");
    QStringList btnObjList = QStringList() << "thumbnail" << "bookmark" << "annotation";

    int nSize = btnStrList.size();
    for (int iLoop = 0; iLoop < nSize; iLoop++) {
        QString sBtn = btnStrList.at(iLoop);
        QString sObj = btnObjList.at(iLoop);
        auto btn = createBtn(sBtn, sObj);
        btnGroup->addButton(btn, iLoop);
        hboxLayout->addWidget(btn);
    }

    m_pHideBtn = new DPushButton(this);
    m_pHideBtn->setVisible(false);
    m_pHideBtn->setCheckable(true);
    btnGroup->addButton(m_pHideBtn);
    hboxLayout->addWidget(m_pHideBtn);

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

DIconButton *MainOperationWidget::createBtn(const QString &btnName, const QString &objName)
{
    auto btn = new DIconButton(this);
    btn->setToolTip(btnName);
    btn->setObjectName(objName);
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
    connect(this, SIGNAL(sigSearchControl()), SLOT(slotSearchControl()));
    connect(this, SIGNAL(sigSearchClosed()), SLOT(slotSearchClosed()));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
}

//  主题更新
void MainOperationWidget::slotUpdateTheme()
{
    auto btnList = this->findChildren<DIconButton *>();
    foreach (auto btn, btnList) {
        QString objName = btn->objectName();
        if (objName != "") {
            QString sPixmap = PF::getImagePath(objName, Pri::g_frame);
            btn->setIcon(QIcon(sPixmap));
        }
    }
    DPalette plt=DGuiApplicationHelper::instance()->applicationPalette();
    plt.setColor(QPalette::Background, plt.color(QPalette::Base));
    setAutoFillBackground(true);
    setPalette(plt);
}

void MainOperationWidget::slotButtonClicked(int id)
{
    m_nThumbnailIndex = id;
    sendMsg(MSG_SWITCHLEFTWIDGET, QString::number(id));
}

/**
 * @brief MainOperationWidget::slotSearchControl
 * 搜索内容时，此模块不可用
 */
void MainOperationWidget::slotSearchControl()
{
    m_pHideBtn->setChecked(true);
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
            break;
        }
    }
}

int MainOperationWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_FIND_CONTENT) {        //  查询内容
        emit sigSearchControl();
    } else if (msgType == MSG_CLEAR_FIND_CONTENT) {
        emit sigSearchClosed();
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }

    return 0;
}
