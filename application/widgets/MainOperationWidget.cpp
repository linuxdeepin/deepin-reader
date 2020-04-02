#include "MainOperationWidget.h"

#include <DToolButton>

#include <QButtonGroup>
#include <QHBoxLayout>

#include "utils/PublicFunction.h"
#include "DocSheet.h"

MainOperationWidget::MainOperationWidget(DocSheet *sheet, DWidget *parent)
    : CustomWidget(MAIN_OPERATION_WIDGET, parent), m_sheet(sheet)
{
    initWidget();
    initConnect();
    slotUpdateTheme();

    dApp->m_pModelService->addObserver(this);
}

MainOperationWidget::~MainOperationWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

void MainOperationWidget::initWidget()
{
    auto mLayout = new QHBoxLayout;
    mLayout->setContentsMargins(15, 0, 15, 0);
    mLayout->setSpacing(31);

    auto btnGroup = new QButtonGroup(this);  //  按钮组，　自动实现按钮唯一check属性
    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotButtonClicked(int)));

    QStringList btnStrList = QStringList() << tr("Thumbnails") << tr("Catalog") << tr("Bookmarks") << tr("Annotations");
    QStringList btnObjList = QStringList() << "thumbnail" << "catalog" << "bookmark" << "annotation";

    int nSize = btnStrList.size();
    for (int iLoop = 0; iLoop < nSize; iLoop++) {
        QString sBtn = btnStrList.at(iLoop);
        QString sObj = btnObjList.at(iLoop);
        auto btn = createBtn(sBtn, sObj);
        btnGroup->addButton(btn, iLoop);
        mLayout->addWidget(btn);

        if (iLoop != nSize - 1) {   //  不是最后一个按钮, 则添加弹簧
            mLayout->addStretch();
        }
    }

    auto pSearchBtn = __CreateHideBtn();
    btnGroup->addButton(pSearchBtn, WIDGET_SEARCH);

    this->setLayout(mLayout);
}

//  创建 隐形的按钮, 搜索 和 搜索转圈圈
DPushButton *MainOperationWidget::__CreateHideBtn()
{
    auto pBtn = new DPushButton(this);
    pBtn->setVisible(false);
    pBtn->setCheckable(true);

    return pBtn;
}

void MainOperationWidget::__SetBtnCheckById(const int &id)
{
    auto btnGroup = this->findChild<QButtonGroup *>();
    if (btnGroup) {
        auto btn = btnGroup->button(id);
        if (btn) {
            btn->setChecked(true);
        }

        slotButtonClicked(id);
    }
}

DToolButton *MainOperationWidget::createBtn(const QString &btnName, const QString &objName)
{
    auto btn = new DToolButton(this);
    btn->setToolTip(btnName);
    btn->setObjectName(objName);
    int tW = 36;
    int tH = 36;
    dApp->adaptScreenView(tW, tH);
    btn->setFixedSize(QSize(tW, tH));
    btn->setIconSize(QSize(tW, tH));
    btn->setCheckable(true);
    btn->setChecked(false);

    return btn;
}

void MainOperationWidget::initConnect()
{
}

void MainOperationWidget::handleOpenSuccess()
{
    if (m_sheet.isNull())
        return;

    FileDataModel fdm = m_sheet->qGetFileData();
    int nId = static_cast<int>(fdm.qGetData(LeftIndex));
    if (nId == -1) {
        nId = 0;
    }

    auto btnGroup = this->findChild<QButtonGroup *>();
    if (btnGroup) {
        auto btn = btnGroup->button(nId);
        if (btn) {
            btn->setChecked(true);
        }
    }
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

//  点击按钮 跳转对应的 widget
void MainOperationWidget::slotButtonClicked(int id)
{
    emit sigShowStackWidget(id);
}

int MainOperationWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        handleOpenSuccess();
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        slotUpdateTheme();
    }

    if (m_pMsgList.contains(msgType)) {
        return MSG_OK;
    }
    return MSG_NO_OK;
}

void MainOperationWidget::SetFindOperation(const int &iType)
{
    if (iType == E_FIND_CONTENT) {
        __SetBtnCheckById(WIDGET_SEARCH);
    } else if (iType == E_FIND_EXIT) {
        handleOpenSuccess();
    }
}
