#include "TitleWidget.h"

#include <QHBoxLayout>
#include <QJsonObject>
#include <QWidgetAction>

#include "MsgModel.h"

#include "controller/FileDataManager.h"
#include "menu/FontMenu.h"
#include "menu/ScaleMenu.h"
#include "menu/HandleMenu.h"
#include "utils/PublicFunction.h"

TitleWidget::TitleWidget(CustomWidget *parent)
    : CustomWidget("TitleWidget", parent)
{
    shortKeyList = QStringList() << KeyStr::g_alt_1 << KeyStr::g_alt_2 << KeyStr::g_ctrl_m
                   << KeyStr::g_alt_z ;
    initWidget();
    initConnections();
    slotUpdateTheme();

    dApp->m_pModelService->addObserver(this);
}

TitleWidget::~TitleWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

//  显示了侧边栏, 则隐藏
void TitleWidget::slotSetFindWidget(const int &iFlag)
{
    if (iFlag == 1) {
        m_pThumbnailBtn->setChecked(true);

        MsgModel mm;
        mm.setMsgType(MSG_WIDGET_THUMBNAILS_VIEW);
        mm.setValue("1");

        notifyMsg(E_FILE_MSG, mm.toJson());
    } else {
        slotAppFullScreen();
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
    m_pPreBtn->setDisabled(bAble);
    m_pScaleMenuBtn->setDisabled(bAble);
    m_pNextBtn->setDisabled(bAble);
}

void TitleWidget::slotOpenFileOk(const QString &sPath)
{
    SetBtnDisable(false);

    FileDataModel fdm = dApp->m_pDataManager->qGetFileData(sPath);

    int nState = fdm.qGetData(Thumbnail);
    bool showLeft = nState == 1 ? true : false;

    m_pThumbnailBtn->setChecked(showLeft);
}

// 应用全屏显示
void TitleWidget::slotAppFullScreen()
{
    //  显示了侧边栏, 则隐藏
    m_pThumbnailBtn->setChecked(false);

    MsgModel mm;
    mm.setMsgType(MSG_WIDGET_THUMBNAILS_VIEW);
    mm.setValue("0");

    notifyMsg(E_FILE_MSG, mm.toJson());
}

//  退出放大鏡
void TitleWidget::slotMagnifierCancel()
{
//  取消放大镜
    notifyMsg(MSG_MAGNIFYING, "0");
}

void TitleWidget::initWidget()
{
    initBtns();

    __InitHandel();
    __InitSelectTool();
    __InitScale();

    auto m_layout = new QHBoxLayout();
    m_layout->setContentsMargins(5, 0, 0, 0);
    m_layout->setSpacing(10);
    setLayout(m_layout);

    m_layout->addWidget(m_pThumbnailBtn);
    m_layout->addWidget(m_pSettingBtn);
    m_layout->addWidget(m_pHandleShapeBtn);
    m_layout->addWidget(m_pPreBtn);
    m_layout->addWidget(m_pScaleMenuBtn);
    m_layout->addWidget(m_pNextBtn);

    m_layout->addStretch(1);
}

void TitleWidget::initConnections()
{
    connect(this, SIGNAL(sigSetFindWidget(const int &)), SLOT(slotSetFindWidget(const int &)));
    connect(this, SIGNAL(sigOpenFileOk(const QString &)), SLOT(slotOpenFileOk(const QString &)));
    connect(this, SIGNAL(sigMagnifierCancel()), SLOT(slotMagnifierCancel()));
    connect(this, SIGNAL(sigAppFullScreen()), SLOT(slotAppFullScreen()));

    connect(this, SIGNAL(sigDealWithKeyMsg(const QString &)),
            SLOT(SlotDealWithShortKey(const QString &)));

    connect(this, SIGNAL(sigTabMsg(const QString &)), SLOT(SlotTabMsg(const QString &)));
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

    MsgModel mm;
    mm.setMsgType(MSG_WIDGET_THUMBNAILS_VIEW);
    mm.setValue(QString::number(rl));

    notifyMsg(E_FILE_MSG, mm.toJson());
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

// 比例按钮
void TitleWidget::SlotScaleMenuBtnClicked()
{
    if (m_pScaleMenu && m_pScaleMenuBtn) {
        QPoint point = m_pScaleMenuBtn->mapToGlobal(QPoint(0, 0));
        int nOldY = point.y();

        int nHeight = m_pScaleMenuBtn->height();
        point.setY(nHeight + nOldY + 2);

        m_pScaleMenu->exec(point);
    }
}

void TitleWidget::SlotSetCurrentTool(const QString &sAction)
{
    //  切换了选择工具, 需要取消放大镜的操作
    slotMagnifierCancel();

    if (sAction == "defaultshape") {
        setDefaultShape();
    } else {
        setHandleShape();
    }
}

void TitleWidget::SlotTabMsg(const QString &sContent)
{
    MsgModel mm;
    mm.fromJson(sContent);

    QString sPath = mm.getPath();
    int nMsg = mm.getMsgType();
    if (nMsg == MSG_TAB_SHOW_FILE_CHANGE) {
        OnFileShowChange(sPath);
    }
}

//  处理 快捷键
void TitleWidget::SlotDealWithShortKey(const QString &sKey)
{
    if (sKey == KeyStr::g_alt_1) {  //  选择工具
        slotMagnifierCancel();

        if (0 != m_nCurHandleShape) {
            setDefaultShape();
        }
    } else if (sKey == KeyStr::g_alt_2) {  //  手型工具
        slotMagnifierCancel();

        if (1 != m_nCurHandleShape) {
            setHandleShape();
        }
    } else if (sKey == KeyStr::g_ctrl_m) {  //  显示缩略图
        m_pThumbnailBtn->setChecked(true);
        MsgModel mm;
        mm.setMsgType(MSG_WIDGET_THUMBNAILS_VIEW);
        mm.setValue("1");
        notifyMsg(E_FILE_MSG, mm.toJson());
    } else if (sKey == KeyStr::g_alt_z) {  //  开启放大镜
        setMagnifierState();
    }
}

void TitleWidget::initBtns()
{
    m_pThumbnailBtn = createBtn(tr("Thumbnails"), true);
    m_pThumbnailBtn->setObjectName("thumbnails");
    connect(m_pThumbnailBtn, SIGNAL(clicked()), SLOT(on_thumbnailBtn_clicked()));
}

void TitleWidget::__InitHandel()
{
    m_pHandleMenu = new HandleMenu(this);
    connect(m_pHandleMenu, SIGNAL(sigCurrentTool(const QString &)), SLOT(SlotSetCurrentTool(const QString &)));

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
    m_pScaleMenu = new ScaleMenu(this);

    m_pPreBtn = new DIconButton(DStyle::SP_DecreaseElement);
    m_pPreBtn->setDisabled(true);
    m_pPreBtn->setFixedSize(QSize(24, 24));
    connect(m_pPreBtn, SIGNAL(clicked()), m_pScaleMenu, SLOT(sloPrevScale()));

    m_pScaleMenuBtn = new DPushButton("0%");
    m_pScaleMenuBtn->setDisabled(true);
    connect(m_pScaleMenuBtn, SIGNAL(clicked()), SLOT(SlotScaleMenuBtnClicked()));

    m_pNextBtn = new DIconButton(DStyle::SP_IncreaseElement);
    m_pNextBtn->setDisabled(true);
    m_pNextBtn->setFixedSize(QSize(24, 24));
    connect(m_pNextBtn, SIGNAL(clicked()), m_pScaleMenu, SLOT(sloNextScale()));
}

void TitleWidget::setDefaultShape()
{
    QString btnName = "defaultshape";

    m_pHandleShapeBtn->setToolTip(tr("Select Text"));

    m_nCurHandleShape = 0;

    QIcon icon = PF::getIcon(Pri::g_module + btnName);
    m_pHandleShapeBtn->setIcon(icon);

    notifyMsg(MSG_HANDLESHAPE, QString::number(m_nCurHandleShape));
}

void TitleWidget::setHandleShape()
{
    QString btnName = "handleshape";

    m_pHandleShapeBtn->setToolTip(tr("Hand Tool"));

    m_nCurHandleShape = 1;

    QIcon icon = PF::getIcon(Pri::g_module + btnName);
    m_pHandleShapeBtn->setIcon(icon);

    notifyMsg(MSG_HANDLESHAPE, QString::number(m_nCurHandleShape));
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
    notifyMsg(MSG_MAGNIFYING, QString::number(1));

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
    if (msgType == MSG_FIND_START) {
        emit sigSetFindWidget(1);
    } else if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigOpenFileOk(msgContent);
    } else if (msgType == MSG_OPERATION_SLIDE) {
        emit sigAppFullScreen();
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    } else if (msgType == MSG_MAGNIFYING_CANCEL) {  //  右键取消放大镜
        emit sigMagnifierCancel();
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_HIDE_FIND_WIDGET) {
        emit sigSetFindWidget(0);
    } else if (msgType == E_TAB_MSG) {
        emit sigTabMsg(msgContent);
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == KeyStr::g_esc) {
            emit sigMagnifierCancel();  //  退出放大镜模式
        } else if (msgContent == KeyStr::g_f11) {
            emit sigAppFullScreen();
        } else {
            if (shortKeyList.contains(msgContent)) {
                emit sigDealWithKeyMsg(msgContent);
                return ConstantMsg::g_effective_res;
            }
        }
    }
    return 0;
}
