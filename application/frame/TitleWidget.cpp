#include "TitleWidget.h"
#include <QHBoxLayout>
#include <QWidgetAction>
#include "controller/AppSetting.h"
#include "controller/DataManager.h"

TitleWidget::TitleWidget(CustomWidget *parent)
    : CustomWidget("TitleWidget", parent)
{
    shortKeyList = QStringList() << KeyStr::g_alt_1 << KeyStr::g_alt_2 << KeyStr::g_ctrl_m
                   << KeyStr::g_alt_z;
    initWidget();
    initConnections();
    slotUpdateTheme();

    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

TitleWidget::~TitleWidget()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

//  显示了侧边栏, 则隐藏
void TitleWidget::slotSetFindWidget(const int &iFlag)
{
    if (iFlag == 1) {
        m_pThumbnailBtn->setChecked(true);
    } else {
        slotAppFullScreen();
    }
    //    m_pThumbnailBtn->setStatus(m_pThumbnailBtn->isChecked());
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
void TitleWidget::slotOpenFileOk()
{
    m_pThumbnailBtn->setDisabled(false);
    m_pSettingBtn->setDisabled(false);
    m_pHandleShapeBtn->setDisabled(false);
    m_pMagnifierBtn->setDisabled(false);
}

// 应用全屏显示
void TitleWidget::slotAppFullScreen()
{
    //  显示了侧边栏, 则隐藏
    m_pThumbnailBtn->setChecked(false);

    //  侧边栏 隐藏
    notifyMsgToSubject(MSG_SLIDER_SHOW_STATE, "0");
}

//  退出放大鏡
void TitleWidget::slotMagnifierCancel()
{
    //  如果开启了, 放大镜 则取消
    bool bCheck = m_pMagnifierBtn->isChecked();
    if (bCheck) {
        m_pMagnifierBtn->setChecked(!bCheck);

        //  取消放大镜
        notifyMsgToSubject(MSG_MAGNIFYING, "0");
    }
    //    m_pMagnifierBtn->setStatus(m_pMagnifierBtn->isChecked());
}

void TitleWidget::initWidget()
{
    initBtns();

    initMenus();

    auto m_layout = new QHBoxLayout();
    m_layout->setContentsMargins(5, 0, 0, 0);
    m_layout->setSpacing(10);
    setLayout(m_layout);

    m_layout->addWidget(m_pThumbnailBtn);
    m_layout->addWidget(m_pSettingBtn);
    m_layout->addWidget(m_pHandleShapeBtn);
    m_layout->addWidget(m_pMagnifierBtn);
    m_layout->addStretch(1);
}

void TitleWidget::initConnections()
{
    connect(this, SIGNAL(sigSetFindWidget(const int &)), SLOT(slotSetFindWidget(const int &)));
    connect(this, SIGNAL(sigOpenFileOk()), SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigMagnifierCancel()), SLOT(slotMagnifierCancel()));
    connect(this, SIGNAL(sigAppFullScreen()), SLOT(slotAppFullScreen()));

    connect(this, SIGNAL(sigDealWithShortKey(const QString &)),
            SLOT(slotDealWithShortKey(const QString &)));
}

//  缩略图
void TitleWidget::on_thumbnailBtn_clicked()
{
    bool rl = m_pThumbnailBtn->isChecked();
    notifyMsgToSubject(MSG_SLIDER_SHOW_STATE, QString::number(rl));

    AppSetting::instance()->setKeyValue(KEY_M, QString("%1").arg(rl));
    DataManager::instance()->setBThumbnIsShow(rl);
}

//  文档显示
void TitleWidget::on_settingBtn_clicked()
{
    QPoint point = m_pSettingBtn->mapToGlobal(QPoint(0, 0));
    int nOldY = point.y();

    int nHeight = m_pSettingBtn->height();
    point.setY(nHeight + nOldY + 2);

    m_pSettingMenu->exec(point);
}

//  手型点击
void TitleWidget::on_handleShapeBtn_clicked()
{
    int nHeight = m_pHandleShapeBtn->height();

    QPoint point = m_pHandleShapeBtn->mapToGlobal(QPoint(0, 0));
    int nOldY = point.y();

    point.setY(nHeight + nOldY + 2);
    m_pHandleMenu->exec(point);
}

//  放大镜
void TitleWidget::on_magnifyingBtn_clicked()
{
    bool bCheck = m_pMagnifierBtn->isChecked();
    notifyMsgToSubject(MSG_MAGNIFYING, QString::number(bCheck));

    //  开启了放大镜, 需要把选择工具 切换为 选择工具
    if (bCheck) {
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
}

void TitleWidget::slotActionTrigger(QAction *action)
{
    //  切换了选择工具, 需要取消放大镜的操作
    slotMagnifierCancel();

    QString sAction = action->objectName();
    if (sAction == "defaultshape") {
        setDefaultShape();
    } else {
        setHandleShape();
    }
}

//  处理 快捷键
void TitleWidget::slotDealWithShortKey(const QString &sKey)
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
        //        m_pThumbnailBtn->setFlat(true);
        m_pThumbnailBtn->setChecked(true);
        //        m_pThumbnailBtn->setStatus(m_pThumbnailBtn->isChecked());
        notifyMsgToSubject(MSG_SLIDER_SHOW_STATE, QString::number(1));
        DataManager::instance()->setBThumbnIsShow(1);
    } else if (sKey == KeyStr::g_alt_z) {  //  开启放大镜
        setMagnifierState();
    }
}

void TitleWidget::initBtns()
{
    m_pThumbnailBtn = createBtn(tr("Thumbnails"), true);
    m_pThumbnailBtn->setObjectName("thumbnails");
    connect(m_pThumbnailBtn, SIGNAL(clicked()), SLOT(on_thumbnailBtn_clicked()));

    m_pSettingBtn = createBtn(tr("Page Display"));
    m_pSettingBtn->setObjectName("viewchange");
    connect(m_pSettingBtn, SIGNAL(clicked()), SLOT(on_settingBtn_clicked()));

    m_pHandleShapeBtn = createBtn(tr("Select Text"));
    m_pHandleShapeBtn->setObjectName("defaultshape");
    m_pHandleShapeBtn->setFixedSize(QSize(42, 36));
    m_pHandleShapeBtn->setIconSize(QSize(42, 36));
    connect(m_pHandleShapeBtn, SIGNAL(clicked()), SLOT(on_handleShapeBtn_clicked()));

    m_pMagnifierBtn = createBtn(tr("Magnifier"), true);
    m_pMagnifierBtn->setObjectName("magnifier");
    connect(m_pMagnifierBtn, SIGNAL(clicked()), SLOT(on_magnifyingBtn_clicked()));
}

void TitleWidget::initMenus()
{
    {
        m_pSettingMenu = new DMenu(this);
//        m_pSettingMenu->setFixedWidth(220);
        DFontSizeManager::instance()->bind(m_pSettingMenu, DFontSizeManager::T6);

        auto action = new QWidgetAction(this);
        auto scaleWidget = new FontWidget(this);

        action->setDefaultWidget(scaleWidget);

        m_pSettingMenu->addAction(action);
    }
    {
        m_pHandleMenu = new DMenu(this);

        auto actionGroup = new QActionGroup(this);

        {
            auto action = new QAction(tr("Select Text"), this);
            action->setObjectName("defaultshape");
            action->setCheckable(true);
            action->setChecked(true);

            actionGroup->addAction(action);
            m_pHandleMenu->addAction(action);
            m_pHandleMenu->addSeparator();
        }

        {
            auto action = new QAction(tr("Hand Tool"), this);
            action->setObjectName("handleshape");
            action->setCheckable(true);

            actionGroup->addAction(action);
            m_pHandleMenu->addAction(action);
        }

        connect(actionGroup, SIGNAL(triggered(QAction *)), SLOT(slotActionTrigger(QAction *)));
    }
}

void TitleWidget::setDefaultShape()
{
    QString btnName = "defaultshape";

    auto action = this->findChild<QAction *>(btnName);
    if (action) {
        action->setChecked(true);
    }

    m_pHandleShapeBtn->setToolTip(tr("Select Text"));

    m_nCurHandleShape = 0;

    QIcon icon = PF::getIcon(Pri::g_module + btnName);
    m_pHandleShapeBtn->setIcon(icon);

    notifyMsgToSubject(MSG_HANDLESHAPE, QString::number(m_nCurHandleShape));
}

void TitleWidget::setHandleShape()
{
    QString btnName = "handleshape";
    auto action = this->findChild<QAction *>(btnName);
    if (action) {
        action->setChecked(true);
    }

    m_pHandleShapeBtn->setToolTip(tr("Hand Tool"));

    m_nCurHandleShape = 1;

    QIcon icon = PF::getIcon(Pri::g_module + btnName);
    m_pHandleShapeBtn->setIcon(icon);

    notifyMsgToSubject(MSG_HANDLESHAPE, QString::number(m_nCurHandleShape));
}

DPushButton *TitleWidget::createBtn(const QString &btnName, bool bCheckable)
{
    auto btn = new DPushButton(this);
    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(36, 36));
    btn->setToolTip(btnName);
    btn->setCheckable(bCheckable);
    //    btn->setFlat(true);

    if (bCheckable) {
        btn->setChecked(false);
    }

    btn->setDisabled(true);

    return btn;
}

void TitleWidget::sendMsgToSubject(const int &msgType, const QString &msgCotent)
{
    sendMsg(msgType, msgCotent);
}

void TitleWidget::notifyMsgToSubject(const int &msgType, const QString &msgCotent)
{
    notifyMsg(msgType, msgCotent);
}

//  开启放大镜
void TitleWidget::setMagnifierState()
{
    bool bCheck = m_pMagnifierBtn->isChecked();
    if (!bCheck) {
        m_pMagnifierBtn->setChecked(true);
        notifyMsgToSubject(MSG_MAGNIFYING, QString::number(1));

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
}

//  处理 推送消息
int TitleWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_FIND_CONTENT) {
        emit sigSetFindWidget(1);
    } else if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigOpenFileOk();
    } else if (msgType == MSG_OPERATION_SLIDE) {
        emit sigAppFullScreen();
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    } else if (msgType == MSG_MAGNIFYING_CANCEL) {  //  右键取消放大镜
        emit sigMagnifierCancel();
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_HIDE_FIND_WIDGET) {
        emit sigSetFindWidget(0);
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == KeyStr::g_esc) {
            emit sigMagnifierCancel();  //  退出放大镜模式
        } else if (msgContent == KeyStr::g_f11) {
            emit sigAppFullScreen();
        } else {
            if (shortKeyList.contains(msgContent)) {
                emit sigDealWithShortKey(msgContent);
                return ConstantMsg::g_effective_res;
            }
        }
    }
    return 0;
}
