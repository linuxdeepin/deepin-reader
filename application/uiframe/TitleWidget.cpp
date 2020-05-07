#include "TitleWidget.h"

#include <QHBoxLayout>

#include "widgets/ScaleWidget.h"
#include "widgets/FontMenu.h"
#include "widgets/HandleMenu.h"
#include "TitleMenu.h"
#include "utils/PublicFunction.h"
#include "DocSheet.h"

TitleWidget::TitleWidget(DWidget *parent)
    : CustomWidget(parent)
{
    initWidget();
    slotUpdateTheme();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &TitleWidget::slotUpdateTheme);
}

TitleWidget::~TitleWidget()
{

}

//  主题变了
void TitleWidget::slotUpdateTheme()
{
    auto btnList = this->findChildren<DPushButton *>();
    foreach (auto btn, btnList) {
        if (btn) {
            QString objName = btn->objectName();
            if (objName != "") {
                QIcon icon = PF::getIcon(Pri::g_module + objName);
                btn->setIcon(icon);
            }
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

    setDefaultShape();
}

void TitleWidget::OnShortCut_Alt2()
{
    if (m_curSheet.isNull())
        return;

    m_curSheet->quitMagnifer();

    setHandleShape();
}

void TitleWidget::OnShortCut_CtrlM()
{
    if (m_curSheet.isNull())
        return;

    m_pThumbnailBtn->setChecked(true);

    m_curSheet->setOper(Thumbnail, "1");

    m_curSheet->setSidebarVisible(true);
}

void TitleWidget::initWidget()
{
    initBtns();

    __InitHandel();
    __InitSelectTool();

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

    if (nullptr == m_curSheet || m_curSheet->type() == Dr::Unknown) {
        SetBtnDisable(true);
        return;

    } else if (Dr::PDF == m_curSheet->type()) {

        SetBtnDisable(false);

        int nState = m_curSheet->getOper(Thumbnail).toInt();

        bool showLeft = nState == 1 ? true : false;

        m_pThumbnailBtn->setChecked(showLeft);

        SetBtnDisable(false);

        m_pSw->setSheet(m_curSheet);

        m_pHandleMenu->setHandShape(m_curSheet->isMouseHand());

        if (m_curSheet->isMouseHand())
            setHandleShape();
        else
            setDefaultShape();
    } else if (Dr::DjVu == m_curSheet->type()) {
        SetBtnDisable(false);

        m_pSw->setSheet(m_curSheet);
    }
}

//  缩略图
void TitleWidget::on_thumbnailBtn_clicked()
{
    if (m_curSheet.isNull())
        return;

    bool rl = m_pThumbnailBtn->isChecked();

    m_curSheet->setOper(Thumbnail, QString::number(rl));

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

void TitleWidget::slotFindOperation(const int &sAction)
{
    if (sAction == E_FIND_CONTENT) {
        if (m_pThumbnailBtn) {
            m_pThumbnailBtn->setChecked(true);
        }
    } else if (sAction == E_FIND_EXIT) {
        if (m_curSheet) {
            bool close = m_curSheet->getOper(Thumbnail).toBool();
            m_pThumbnailBtn->setChecked(close);
        }
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

    m_pSearchBtn->setFixedSize(QSize(tW, tH));
    tW = 30;
    tH = 30;

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

    if (m_pHandleMenu) {
        m_pHandleMenu->setHandShape(0);
    }
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

    if (m_pHandleMenu) {
        m_pHandleMenu->setHandShape(1);
    }
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

/**
 * @brief TitleWidget::setControlEnabled
 * 设置TitleWidget中空间的可用或者不可用
 * @param enable  true:可用      false:不可用
 */
void TitleWidget::setControlEnabled(const bool &enable)
{
    if (m_pThumbnailBtn != nullptr) {
        m_pThumbnailBtn->setChecked(false);
        m_pThumbnailBtn->setEnabled(enable);
    }
    if (m_pSettingBtn != nullptr) {
        m_pSettingBtn->setChecked(false);
        m_pSettingBtn->setEnabled(enable);
    }
    if (m_pHandleShapeBtn != nullptr) {
        m_pHandleShapeBtn->setChecked(false);
        m_pHandleShapeBtn->setEnabled(enable);
    }
    if (m_pSearchBtn != nullptr) {
        m_pSearchBtn->setChecked(false);
        m_pSearchBtn->setEnabled(enable);
    }
    if (m_pSw) {
        m_pSw->clearComboBox();
    }
}

int TitleWidget::handleShortcut(const QString &sKey)
{
    if (sKey == KeyStr::g_alt_1) {
        OnShortCut_Alt1();
        return MSG_OK;
    } else if (sKey == KeyStr::g_alt_2) {
        OnShortCut_Alt2();
        return MSG_OK;
    } else if (sKey == KeyStr::g_ctrl_m) { //  显示缩略图
        OnShortCut_CtrlM();
        return MSG_OK;
    } else {
        m_pFontMenu->readCurDocParam(m_curSheet.data());
        if (m_pFontMenu->handleShortcut(sKey))
            return MSG_OK;

        if (m_pSw->handleShortcut(sKey))
            return MSG_OK;
    }

    return MSG_NO_OK;
}
