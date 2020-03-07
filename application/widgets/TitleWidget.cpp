#include "TitleWidget.h"

#include <QHBoxLayout>
#include <QWidgetAction>
#include <DComboBox>
#include <QLineEdit>

#include "ScaleWidget.h"

#include "menu/FontMenu.h"
//#include "menu/ScaleMenu.h"
#include "menu/HandleMenu.h"
#include "main/MainTabWidgetEx.h"
#include "menu/TitleMenu.h"
#include "utils/PublicFunction.h"

TitleWidget *TitleWidget::g_onlyTitleWdiget = nullptr;

TitleWidget *TitleWidget::Instance()
{
    return g_onlyTitleWdiget;
}

TitleWidget::TitleWidget(CustomWidget *parent)
    : CustomWidget(TITLE_WIDGET, parent)
{
    m_pMsgList = {MSG_TAB_SHOW_FILE_CHANGE, E_FIND_CONTENT, E_FIND_EXIT};

    shortKeyList = QStringList() << KeyStr::g_alt_1 << KeyStr::g_alt_2 << KeyStr::g_ctrl_m
                   << KeyStr::g_alt_z
                   << KeyStr::g_ctrl_1 << KeyStr::g_ctrl_2 << KeyStr::g_ctrl_3
                   << KeyStr::g_ctrl_r << KeyStr::g_ctrl_shift_r
                   << KeyStr::g_ctrl_larger << KeyStr::g_ctrl_equal << KeyStr::g_ctrl_smaller;

    initWidget();
    initConnections();
    slotUpdateTheme();

    g_onlyTitleWdiget = this;

    dApp->m_pModelService->addObserver(this);
}

TitleWidget::~TitleWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

//  缩略图显示按钮状态切换
void TitleWidget::SetFindWidget(const int &iFlag, const QString &sPath)
{
    if (iFlag == E_FIND_CONTENT) {
        m_pThumbnailBtn->setChecked(true);
    } else if (iFlag == E_FIND_EXIT) {
        FileDataModel fdm = MainTabWidgetEx::Instance()->qGetFileData(sPath);

        int nState = fdm.qGetData(Thumbnail);
        bool showLeft = nState == 1 ? true : false;

        m_pThumbnailBtn->setChecked(showLeft);
    }
}

//  主题变了
void TitleWidget::slotUpdateTheme()
{
    auto btnList = this->findChildren<DPushButton *>();
    foreach (auto btn, btnList) {
        QString objName = btn->objectName();
        if (objName != "") {
            QIcon icon = PF::getIcon(Pri::g_module + objName);
            btn->setIcon(icon);
        }
    }

    auto actionList = this->findChildren<QAction *>();
    foreach (auto a, actionList) {
        QString objName = a->objectName();
        if (objName != "") {
            QIcon icon = PF::getIcon(Pri::g_module + objName + "_small");
            a->setIcon(icon);
        }
    }
}

//  打开文件成功
void TitleWidget::SetBtnDisable(const bool &bAble)
{
    m_pThumbnailBtn->setDisabled(bAble);
    m_pSettingBtn->setDisabled(bAble);
    m_pHandleShapeBtn->setDisabled(bAble);
//    m_pPreBtn->setDisabled(bAble);
//    m_pScaleMenuBtn->setDisabled(bAble);
//    m_pNextBtn->setDisabled(bAble);
    m_pSearchBtn->setDisabled(bAble);

    m_pSw->setDisabled(bAble);
}

void TitleWidget::OnShortCut_Alt1()
{
    MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
    if (pMtwe) {
        pMtwe->OnExitMagnifer();

        int nState = pMtwe->getCurrentState();

        if (Default_State != nState) {
            setDefaultShape();
        }
    }
}

void TitleWidget::OnShortCut_Alt2()
{
    MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
    if (pMtwe) {
        pMtwe->OnExitMagnifer();

        int nState = pMtwe->getCurrentState();

        if (Handel_State != nState) {
            setHandleShape();
        }
    }
}

void TitleWidget::OnShortCut_CtrlM()
{
    m_pThumbnailBtn->setChecked(true);

    QJsonObject obj;
    obj.insert("content", "1");
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_WIDGET_THUMBNAILS_VIEW, doc.toJson(QJsonDocument::Compact));
}

void TitleWidget::slotOpenFileOk(const QString &sPath)
{
    SetBtnDisable(false);

    FileDataModel fdm = MainTabWidgetEx::Instance()->qGetFileData(sPath);

    int nState = fdm.qGetData(Thumbnail);
    bool showLeft = nState == 1 ? true : false;

    m_pThumbnailBtn->setChecked(showLeft);
}

void TitleWidget::initWidget()
{
    initBtns();

    __InitHandel();
    __InitSelectTool();
//    __InitScale();

    auto m_layout = new QHBoxLayout();
    m_layout->setContentsMargins(5, 0, 0, 0);
    m_layout->setSpacing(10);
    setLayout(m_layout);

    m_layout->addWidget(m_pThumbnailBtn);
    m_layout->addWidget(m_pSettingBtn);
    m_layout->addWidget(m_pHandleShapeBtn);
//    m_layout->addWidget(m_pPreBtn);
//    m_layout->addWidget(m_pScaleMenuBtn);
//    m_layout->addWidget(m_pNextBtn);

    m_pSw = new ScaleWidget;
    connect(m_pSw, SIGNAL(sigScaleChanged()), SLOT(SlotScaleChanged()));
    m_pSw->setDisabled(true);

    m_layout->addWidget(m_pSw);

    m_layout->addWidget(m_pSearchBtn);
    m_layout->addStretch(1);
}

void TitleWidget::initConnections()
{
//    connect(this, SIGNAL(sigSetFindWidget(const int &)), SLOT(slotSetFindWidget(const int &)));
//    connect(this, SIGNAL(sigMagnifierCancel()), SLOT(slotMagnifierCancel()));
//    connect(this, SIGNAL(sigAppFullScreen()), SLOT(slotAppFullScreen()));
}

//  文档切换了
void TitleWidget::OnFileShowChange(const QString &sPath)
{
    slotOpenFileOk(sPath);
}

//  缩略图
void TitleWidget::on_thumbnailBtn_clicked()
{
    bool rl = m_pThumbnailBtn->isChecked();

    QJsonObject obj;
    obj.insert("content", QString::number(rl));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_WIDGET_THUMBNAILS_VIEW, doc.toJson(QJsonDocument::Compact));
}

//  文档显示
void TitleWidget::on_settingBtn_clicked()
{
    if (m_pFontMenu && m_pSettingBtn) {
        QPoint point = m_pSettingBtn->mapToGlobal(QPoint(0, 0));
        int nOldY = point.y();

        int nHeight = m_pSettingBtn->height();
        point.setY(nHeight + nOldY + 2);

        m_pFontMenu->exec(point);
    }
}

//  手型点击
void TitleWidget::on_handleShapeBtn_clicked()
{
    if (m_pHandleMenu && m_pHandleShapeBtn) {
        int nHeight = m_pHandleShapeBtn->height();

        QPoint point = m_pHandleShapeBtn->mapToGlobal(QPoint(0, 0));
        int nOldY = point.y();

        point.setY(nHeight + nOldY + 2);

        m_pHandleMenu->exec(point);
    }
}

void TitleWidget::on_searchBtn_clicked()
{
    QJsonObject obj;
    obj.insert("type", "ShortCut");
    obj.insert("key", KeyStr::g_ctrl_f);

    QJsonDocument doc = QJsonDocument(obj);
    notifyMsg(E_APP_MSG_TYPE, doc.toJson(QJsonDocument::Compact));
}

// 比例按钮
void TitleWidget::SlotScaleMenuBtnClicked()
{
//    if (m_pScaleMenu && m_pScaleMenuBtn) {
//        QPoint point = m_pScaleMenuBtn->mapToGlobal(QPoint(0, 0));
//        int nOldY = point.y();

//        int nHeight = m_pScaleMenuBtn->height();
//        point.setY(nHeight + nOldY + 2);

//        m_pScaleMenu->exec(point);
//    }
}

void TitleWidget::SlotSetCurrentTool(const int &sAction)
{
    //  切换了选择工具, 需要取消放大镜的操作
    MainTabWidgetEx::Instance()->OnExitMagnifer();

    if (sAction == E_HANDLE_SELECT_TEXT) {
        setDefaultShape();
    } else {
        setHandleShape();
    }
}


void TitleWidget::SlotScaleChanged()
{
//    m_pScaleMenuBtn->setText(QString::number(iScale) + "%");

//    if (iFlag == 0) {
    //  比例变化, 自适应宽\高 状态 取消
    m_pFontMenu->CancelFitState();
//    }
}

void TitleWidget::initBtns()
{
    m_pThumbnailBtn = createBtn(tr("Thumbnails"), true);
    m_pThumbnailBtn->setObjectName("thumbnails");
    connect(m_pThumbnailBtn, SIGNAL(clicked()), SLOT(on_thumbnailBtn_clicked()));

    m_pSearchBtn = new DIconButton(DStyle::SP_IndicatorSearch);
    m_pSearchBtn->setDisabled(true);
    m_pSearchBtn->setFixedSize(QSize(36, 36));
    m_pSearchBtn->setIconSize(QSize(30, 30));
    connect(m_pSearchBtn, SIGNAL(clicked()), SLOT(on_searchBtn_clicked()));
}

void TitleWidget::__InitHandel()
{
    m_pHandleMenu = new HandleMenu(this);
    connect(m_pHandleMenu, SIGNAL(sigClickAction(const int &)), SLOT(SlotSetCurrentTool(const int &)));

    m_pHandleShapeBtn = createBtn(tr("Select Text"));
    m_pHandleShapeBtn->setObjectName("defaultshape");
    m_pHandleShapeBtn->setFixedSize(QSize(42, 36));
    m_pHandleShapeBtn->setIconSize(QSize(42, 36));
    connect(m_pHandleShapeBtn, SIGNAL(clicked()), SLOT(on_handleShapeBtn_clicked()));
}

void TitleWidget::__InitSelectTool()
{
    //字号调整菜单
    m_pFontMenu = new FontMenu(this);

    m_pSettingBtn = createBtn(tr("Page Display"));
    m_pSettingBtn->setObjectName("viewchange");
    connect(m_pSettingBtn, SIGNAL(clicked()), SLOT(on_settingBtn_clicked()));
}

void TitleWidget::__InitScale()
{
//    m_pScaleMenu = new ScaleMenu(this);
//    connect(m_pScaleMenu, SIGNAL(sigCurrentScale(const int &, const int &)), SLOT(SlotCurrentScale(const int &, const int &)));

//    m_pPreBtn = new DIconButton(DStyle::SP_DecreaseElement);
//    m_pPreBtn->setDisabled(true);
//    m_pPreBtn->setFixedSize(QSize(24, 24));
//    connect(m_pPreBtn, SIGNAL(clicked()), m_pScaleMenu, SLOT(slotPrevScale()));

//    m_pScaleMenuBtn = new DPushButton("0%");
//    m_pScaleMenuBtn->setDisabled(true);
//    connect(m_pScaleMenuBtn, SIGNAL(clicked()), SLOT(SlotScaleMenuBtnClicked()));

//    m_pNextBtn = new DIconButton(DStyle::SP_IncreaseElement);
//    m_pNextBtn->setDisabled(true);
//    m_pNextBtn->setFixedSize(QSize(24, 24));
//    connect(m_pNextBtn, SIGNAL(clicked()), m_pScaleMenu, SLOT(slotNextScale()));
}

void TitleWidget::setDefaultShape()
{
    QString btnName = "defaultshape";

    m_pHandleShapeBtn->setToolTip(tr("Select Text"));

    QIcon icon = PF::getIcon(Pri::g_module + btnName);
    m_pHandleShapeBtn->setIcon(icon);

    QJsonObject obj;
    obj.insert("content", QString::number(Default_State));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_HANDLESHAPE, doc.toJson(QJsonDocument::Compact));
}

void TitleWidget::setHandleShape()
{
    QString btnName = "handleshape";

    m_pHandleShapeBtn->setToolTip(tr("Hand Tool"));

    QIcon icon = PF::getIcon(Pri::g_module + btnName);
    m_pHandleShapeBtn->setIcon(icon);

    QJsonObject obj;
    obj.insert("content", QString::number(Handel_State));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_HANDLESHAPE, doc.toJson(QJsonDocument::Compact));
}

DPushButton *TitleWidget::createBtn(const QString &btnName, bool bCheckable)
{
    auto btn = new DPushButton(this);
    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(36, 36));
    btn->setToolTip(btnName);
    btn->setCheckable(bCheckable);

    if (bCheckable) {
        btn->setChecked(false);
    }

    btn->setDisabled(true);

    return btn;
}

//  开启放大镜
void TitleWidget::setMagnifierState()
{
//    MsgModel mm;
//    mm.setMsgType(MSG_MAGNIFYING);
//    mm.setValue("1");

//    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
//    mm.setPath(sCurPath);

//    notifyMsg(MSG_MAGNIFYING, mm.toJson());

    //  开启了放大镜, 需要把选择工具 切换为 选择工具
    auto actionList = this->findChildren<QAction *>();
    foreach (auto a, actionList) {
        QString objName = a->objectName();
        if (objName == "defaultshape") {
            a->setChecked(true);
            break;
        }
    }

    QIcon icon = PF::getIcon(Pri::g_module + "defaultshape");
    m_pHandleShapeBtn->setIcon(icon);
}

//  处理 推送消息
int TitleWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK || msgType == MSG_TAB_SHOW_FILE_CHANGE) {
        OnFileShowChange(msgContent);
        TitleMenu::Instance()->dealWithData(msgType, msgContent);
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        slotUpdateTheme();
    }

    int nRes = m_pSw->dealWithData(msgType, msgContent);
    if (nRes == MSG_OK) {
        return MSG_OK;
    }

    nRes = m_pFontMenu->dealWithData(msgType, msgContent);
    if (nRes == MSG_OK) {
        return MSG_OK;
    }

    if (m_pMsgList.contains(msgType)) {
        return MSG_OK;
    }

    return MSG_NO_OK;
}

int TitleWidget::qDealWithShortKey(const QString &sKey)
{
    if (sKey == KeyStr::g_alt_1) {
        OnShortCut_Alt1();
    } else if (sKey == KeyStr::g_alt_2) {
        OnShortCut_Alt2();
    } else if (sKey == KeyStr::g_ctrl_m) { //  显示缩略图
        OnShortCut_CtrlM();
    } else {
        int nRes = m_pFontMenu->dealWithData(MSG_NOTIFY_KEY_MSG, sKey);
        if (nRes != MSG_OK) {
            nRes = m_pSw->dealWithData(MSG_NOTIFY_KEY_MSG, sKey);

            if (nRes == MSG_OK) {
                return MSG_OK;
            }
        }
    }

    if (m_pKeyMsgList.contains(sKey)) {
        return MSG_OK;
    }

    return MSG_NO_OK;
}
