#include "TitleWidget.h"

#include <QHBoxLayout>

#include "widgets/ScaleWidget.h"
#include "widgets/FontMenu.h"
#include "widgets/HandleMenu.h"
#include "TitleMenu.h"
#include "utils/PublicFunction.h"
#include "DocSheet.h"

TitleWidget::TitleWidget(DWidget *parent)
    : CustomWidget(TITLE_WIDGET, parent)
{
    m_pMsgList = {E_FIND_CONTENT, E_FIND_EXIT};

    shortKeyList = QStringList() << KeyStr::g_alt_1 << KeyStr::g_alt_2 << KeyStr::g_ctrl_m
                   << KeyStr::g_alt_z
                   << KeyStr::g_ctrl_1 << KeyStr::g_ctrl_2 << KeyStr::g_ctrl_3
                   << KeyStr::g_ctrl_r << KeyStr::g_ctrl_shift_r
                   << KeyStr::g_ctrl_larger << KeyStr::g_ctrl_equal << KeyStr::g_ctrl_smaller;

    initWidget();
    slotUpdateTheme();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &TitleWidget::slotUpdateTheme);
    dApp->m_pModelService->addObserver(this);
}

TitleWidget::~TitleWidget()
{
    dApp->m_pModelService->removeObserver(this);
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
    m_pSearchBtn->setDisabled(bAble);
    m_pSw->setDisabled(bAble);
}

void TitleWidget::OnShortCut_Alt1()
{
    if (m_curSheet.isNull())
        return;

    m_curSheet->quitMagnifer();

    int nState = m_curSheet->getCurrentState();

    if (Default_State != nState) {
        setDefaultShape();
        m_curSheet->setMouseDefault();
    }
}

void TitleWidget::OnShortCut_Alt2()
{
    if (m_curSheet.isNull())
        return;

    m_curSheet->quitMagnifer();

    int nState = m_curSheet->getCurrentState();

    if (Handel_State != nState) {
        setHandleShape();
        m_curSheet->setMouseHand();
    }
}

void TitleWidget::OnShortCut_CtrlM()
{
    if (m_curSheet.isNull())
        return;

    m_pThumbnailBtn->setChecked(true);
    m_curSheet->setData(Thumbnail, "1");
    m_curSheet->setSidebarVisible(true);
}

void TitleWidget::slotOpenFileOk(const QString &sPath)
{
    if (m_curSheet.isNull())
        return;

    SetBtnDisable(false);

    FileDataModel fdm = m_curSheet->qGetFileData();

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

    m_pSw = new ScaleWidget;
    m_pSw->setDisabled(true);

    m_layout->addWidget(m_pSw);

    m_layout->addWidget(m_pSearchBtn);
    m_layout->addStretch(1);
}

void TitleWidget::onCurSheetChanged(DocSheet *sheet)
{
    m_curSheet = sheet;

    if (nullptr == m_curSheet || m_curSheet->type() == DocType_NULL) {
        SetBtnDisable(true);

        return;

    } else if (DocType_PDF == m_curSheet->type()) {

        SetBtnDisable(false);

        FileDataModel fdm = m_curSheet->qGetFileData();

        int nState = fdm.qGetData(Thumbnail);

        bool showLeft = nState == 1 ? true : false;

        m_pThumbnailBtn->setChecked(showLeft);

        SetBtnDisable(false);

        m_pSw->setSheet(m_curSheet);

        m_pHandleMenu->setHandShape(m_curSheet->isMouseHand());

        if (m_curSheet->isMouseHand())
            setHandleShape();
        else
            setDefaultShape();
    }
}

//  文档切换了
void TitleWidget::OnFileShowChange(const QString &sPath)
{
    slotOpenFileOk(sPath);
}

//  缩略图
void TitleWidget::on_thumbnailBtn_clicked()
{
    if (m_curSheet.isNull())
        return;

    bool rl = m_pThumbnailBtn->isChecked();
    m_curSheet->setData(Thumbnail, QString::number(rl));
    m_curSheet->setSidebarVisible(rl);
}

//  文档显示
void TitleWidget::on_settingBtn_clicked()
{
    if (m_pFontMenu && m_pSettingBtn) {
        QPoint point = m_pSettingBtn->mapToGlobal(QPoint(0, 0));
        int nOldY = point.y();

        int nHeight = m_pSettingBtn->height();
        point.setY(nHeight + nOldY + 2);

        m_pFontMenu->readCurDocParam(m_curSheet.data());
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
    m_curSheet->handleShortcut(KeyStr::g_ctrl_f);
}

void TitleWidget::SlotSetCurrentTool(const int &sAction)
{
    //  切换了选择工具, 需要取消放大镜的操作
    if (m_curSheet.isNull())
        return;

    m_curSheet->quitMagnifer();

    if (sAction == E_HANDLE_SELECT_TEXT) {
        setDefaultShape();
    } else {
        setHandleShape();
    }
}

void TitleWidget::initBtns()
{
    m_pThumbnailBtn = createBtn(tr("Thumbnails"), true);
    m_pThumbnailBtn->setObjectName("thumbnails");
    connect(m_pThumbnailBtn, SIGNAL(clicked()), SLOT(on_thumbnailBtn_clicked()));

    m_pSearchBtn = new DIconButton(DStyle::SP_IndicatorSearch);
    m_pSearchBtn->setDisabled(true);
    int tW = 36;
    int tH = 36;
//    dApp->adaptScreenView(tW, tH);
    m_pSearchBtn->setFixedSize(QSize(tW, tH));
    tW = 30;
    tH = 30;
//    dApp->adaptScreenView(tW, tH);
    m_pSearchBtn->setIconSize(QSize(tW, tH));
    connect(m_pSearchBtn, SIGNAL(clicked()), SLOT(on_searchBtn_clicked()));
}

void TitleWidget::__InitHandel()
{
    m_pHandleMenu = new HandleMenu(this);
    connect(m_pHandleMenu, SIGNAL(sigClickAction(const int &)), SLOT(SlotSetCurrentTool(const int &)));

    m_pHandleShapeBtn = createBtn(tr("Select Text"));
    m_pHandleShapeBtn->setObjectName("defaultshape");
    int tW = 42;
    int tH = 36;
//    dApp->adaptScreenView(tW, tH);
    m_pHandleShapeBtn->setFixedSize(QSize(tW, tH));
    m_pHandleShapeBtn->setIconSize(QSize(tW, tH));
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

void TitleWidget::setDefaultShape()
{
    if (m_curSheet.isNull())
        return;

    QString btnName = "defaultshape";

    m_pHandleShapeBtn->setToolTip(tr("Select Text"));

    QIcon icon = PF::getIcon(Pri::g_module + btnName);

    m_pHandleShapeBtn->setIcon(icon);

    m_curSheet->setMouseDefault();
}

void TitleWidget::setHandleShape()
{
    if (m_curSheet.isNull())
        return;

    QString btnName = "handleshape";

    m_pHandleShapeBtn->setToolTip(tr("Hand Tool"));

    QIcon icon = PF::getIcon(Pri::g_module + btnName);

    m_pHandleShapeBtn->setIcon(icon);

    m_curSheet->setMouseHand();
}

DPushButton *TitleWidget::createBtn(const QString &btnName, bool bCheckable)
{
    auto btn = new DPushButton(this);
    int tW = 36;
//    int tH = 36;
//    dApp->adaptScreenView(tW, tH);
    btn->setFixedSize(QSize(tW, tW));
    btn->setIconSize(QSize(tW, tW));
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

int TitleWidget::onTitleShortCut(const QString &sKey)
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
