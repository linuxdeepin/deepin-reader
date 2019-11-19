#include "TitleWidget.h"
#include <QHBoxLayout>

#include <QWidgetAction>
#include <DFontSizeManager>

TitleWidget::TitleWidget(CustomWidget *parent) :
    CustomWidget("TitleWidget", parent)
{
    initWidget();
    initConnections();
    slotUpdateTheme();
}

TitleWidget::~TitleWidget()
{

}

//  主题变了
void TitleWidget::slotUpdateTheme()
{
    auto btnList = this->findChildren<DToolButton *>();
    foreach (auto btn, btnList) {
        QString objName = btn->objectName();
        if (objName != "") {
            QString sPixmap = PF::getImagePath(objName, Pri::g_frame);
            btn->setIcon(QIcon(sPixmap));
        }
    }

    auto actionList = this->findChildren<QAction *>();
    foreach (auto a, actionList) {
        QString objName = a->objectName();
        if (objName != "") {
            QString sPixmap = PF::getImagePath(objName + "_small", Pri::g_frame);
            a->setIcon(QIcon(sPixmap));
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
    bool bCheck = m_pThumbnailBtn->isChecked();
    if (bCheck) {
        m_pThumbnailBtn->setChecked(!bCheck);

        //  侧边栏 隐藏
        notifyMsgToSubject(MSG_SLIDER_SHOW_STATE, "0");
    }
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
    connect(this, SIGNAL(sigOpenFileOk()), SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigMagnifierCancel()), SLOT(slotMagnifierCancel()));
    connect(this, SIGNAL(sigAppFullScreen()), SLOT(slotAppFullScreen()));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
    connect(this, SIGNAL(sigDealWithShortKey(const QString &)), SLOT(slotDealWithShortKey(const QString &)));
}

//  缩略图
void TitleWidget::on_thumbnailBtn_clicked()
{
    bool rl = m_pThumbnailBtn->isChecked();
    notifyMsgToSubject(MSG_SLIDER_SHOW_STATE, QString::number(rl));
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
            if (objName == "defaultShape") {
                a->setChecked(true);
                break;
            }
        }
        QString normalPic = PF::getImagePath("defaultShape", Pri::g_frame);
        m_pHandleShapeBtn->setIcon(QIcon(normalPic));
    }
}

void TitleWidget::slotActionTrigger(QAction *action)
{
    //  切换了选择工具, 需要取消放大镜的操作
    slotMagnifierCancel();

    QString sAction = action->objectName();
    if (sAction == "defaultShape") {
        setDefaultShape();
    } else {
        setHandleShape();
    }
}

//  处理 快捷键
void TitleWidget::slotDealWithShortKey(const QString &sKey)
{
    if (sKey == KeyStr::g_alt_1) {          //  选择工具
        slotMagnifierCancel();

        if (0 != m_nCurHandleShape) {
            setDefaultShape();
        }
    } else if (sKey == KeyStr::g_alt_2) {   //  手型工具
        slotMagnifierCancel();

        if (1 != m_nCurHandleShape) {
            setHandleShape();
        }
    } else if ( sKey == KeyStr::g_m) {      //  显示缩略图
        bool rl = m_pThumbnailBtn->isChecked();
        if (!rl) {
            m_pThumbnailBtn->setChecked(true);
            notifyMsgToSubject(MSG_SLIDER_SHOW_STATE, QString::number(1));
        }
    } else if ( sKey == KeyStr::g_z) {      //  开启放大镜
        bool bCheck = m_pMagnifierBtn->isChecked();
        if (!bCheck) {
            m_pMagnifierBtn->setChecked(true);
            notifyMsgToSubject(MSG_MAGNIFYING, QString::number(1));

            //  开启了放大镜, 需要把选择工具 切换为 选择工具
            auto actionList = this->findChildren<QAction *>();
            foreach (auto a, actionList) {
                QString objName = a->objectName();
                if (objName == "defaultShape") {
                    a->setChecked(true);
                    break;
                }
            }
            QString normalPic = PF::getImagePath("defaultShape", Pri::g_frame);
            m_pHandleShapeBtn->setIcon(QIcon(normalPic));
        }
    }
}

void TitleWidget::initBtns()
{
    m_pThumbnailBtn = createBtn(tr("thumbnails"), true);
    m_pThumbnailBtn->setObjectName("thumbnails");
    connect(m_pThumbnailBtn, SIGNAL(clicked()), SLOT(on_thumbnailBtn_clicked()));

    m_pSettingBtn = createBtn(tr("viewchange"));
    m_pSettingBtn->setObjectName("viewchange");
    connect(m_pSettingBtn, SIGNAL(clicked()), SLOT(on_settingBtn_clicked()));

    m_pHandleShapeBtn = createBtn(tr("defaultShape"));
    m_pHandleShapeBtn->setObjectName("defaultShape");
    m_pHandleShapeBtn->setFixedSize(QSize(42, 36));
    m_pHandleShapeBtn->setIconSize(QSize(42, 36));
    connect(m_pHandleShapeBtn, SIGNAL(clicked()), SLOT(on_handleShapeBtn_clicked()));

    m_pMagnifierBtn = createBtn(tr("magnifier"), true);
    m_pMagnifierBtn->setObjectName("magnifier");
    connect(m_pMagnifierBtn, SIGNAL(clicked()), SLOT(on_magnifyingBtn_clicked()));
}

void TitleWidget::initMenus()
{
    {
        m_pSettingMenu = new DMenu(this);
        m_pSettingMenu->setFixedWidth(220);
//        QFont font;
//        font.setPixelSize(14);
//        m_pSettingMenu->setFont(font);
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
            auto action = new QAction(tr("defaultShape"), this);
            action->setObjectName("defaultShape");
            action->setCheckable(true);
            action->setChecked(true);

            actionGroup->addAction(action);
            m_pHandleMenu->addAction(action);
        }

        {
            auto action = new QAction(tr("handleShape"), this);
            action->setObjectName("handleShape");
            action->setCheckable(true);

            actionGroup->addAction(action);
            m_pHandleMenu->addAction(action);
        }

        connect(actionGroup, SIGNAL(triggered(QAction *)), SLOT(slotActionTrigger(QAction *)));
    }
}

void TitleWidget::setDefaultShape()
{
    auto action = this->findChild<QAction *>("defaultShape");
    if (action) {
        action->setChecked(true);
    }

    m_pHandleShapeBtn->setToolTip(tr("defaultShape"));

    m_nCurHandleShape = 0;
    QString btnName = "defaultShape";
    QString normalPic = PF::getImagePath(btnName, Pri::g_frame);
    m_pHandleShapeBtn->setIcon(QIcon(normalPic));

    notifyMsgToSubject(MSG_HANDLESHAPE, QString::number(m_nCurHandleShape));
}

void TitleWidget::setHandleShape()
{
    auto action = this->findChild<QAction *>("handleShape");
    if (action) {
        action->setChecked(true);
    }

    m_pHandleShapeBtn->setToolTip(tr("handleShape"));

    m_nCurHandleShape = 1;
    QString btnName = "handleShape";
    QString normalPic = PF::getImagePath(btnName, Pri::g_frame);
    m_pHandleShapeBtn->setIcon(QIcon(normalPic));

    notifyMsgToSubject(MSG_HANDLESHAPE, QString::number(m_nCurHandleShape));
}

DToolButton *TitleWidget::createBtn(const QString &btnName, bool bCheckable)
{
    auto btn = new DToolButton(this);
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

void TitleWidget::sendMsgToSubject(const int &msgType, const QString &msgCotent)
{
    sendMsg(msgType, msgCotent);
}

void TitleWidget::notifyMsgToSubject(const int &msgType, const QString &msgCotent)
{
    notifyMsg(msgType, msgCotent);
}

//  处理 推送消息
int TitleWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigOpenFileOk();
    } else if (msgType == MSG_OPERATION_FULLSCREEN || msgType == MSG_OPERATION_SLIDE) {
        emit sigAppFullScreen();
    }  else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    } else if (msgType == MSG_MAGNIFYING_CANCEL) {  //  右键取消放大镜
        emit sigMagnifierCancel();
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == KeyStr::g_esc) {      //  退出放大镜模式
            emit sigMagnifierCancel();
        } else if (msgContent == KeyStr::g_alt_1 || msgContent == KeyStr::g_alt_2
                   || msgContent == KeyStr::g_m || msgContent == KeyStr::g_z) { //  选择工具
            emit sigDealWithShortKey(msgContent);
            return ConstantMsg::g_effective_res;
        }
    }
    return 0;
}
