/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "FontMenu.h"
#include <DFontSizeManager>
#include <QWidgetAction>
#include "utils/PublicFunction.h"
#include "subjectObserver/ModuleHeader.h"
#include "controller/AppSetting.h"
#include "frame/DocummentFileHelper.h"

FontMenu::FontMenu(QWidget *parent):
    DMenu(parent)
{
    shortKeyList << KeyStr::g_ctrl_1 << KeyStr::g_ctrl_2 << KeyStr::g_ctrl_3
                 << KeyStr::g_ctrl_r << KeyStr::g_ctrl_shift_r
                 << KeyStr::g_ctrl_larger << KeyStr::g_ctrl_equal << KeyStr::g_ctrl_smaller;

    m_pNotifySubject = g_NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }

    initMenu();

    initConnection();
}

FontMenu::~FontMenu()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

/**
 * @brief FontMenu::dealWithData
 * 处理全局消息
 * @param msgType     消息类型
 * @param msgContent  消息内容
 * @return            处理消息状态
 */
int FontMenu::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigFileOpenOk();
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (shortKeyList.contains(msgContent)) {
            emit sigDealWithShortKey(msgContent);
            return ConstantMsg::g_effective_res;
        }
    } else if (msgType == MSG_SELF_ADAPTE_SCALE) {
        emit sigSetCurScale(msgContent);
    }
    return 0;
}

// 不用此接口
void FontMenu::sendMsg(const int &, const QString &)
{

}

/**
 * @brief FontMenu::notifyMsg
 * 分发全局消息
 * @param msgType     消息类型
 * @param msgContent  消息内容
 */
void FontMenu::notifyMsg(const int &msgType, const QString &msgContent)
{
    if (m_pNotifySubject == nullptr) {
        m_pNotifySubject = g_NotifySubject::getInstance();
    }

    if (m_pNotifySubject != nullptr) {
        m_pNotifySubject->notifyMsg(msgType, msgContent);
    }
}

/**
 * @brief FontMenu::resetAdaptive
 * 手动改变(ctrl + 1)缩放比例时，复位自适应宽高
 */
void FontMenu::resetAdaptive()
{
    resetFiteHAndW();

    m_bIsAdaptMove = true;
    if (m_pEnlargeSlider) {
        m_pEnlargeSlider->setValue(100);
    }
    if (m_pEnlargeLab) {
        m_pEnlargeLab->setText(QString("%1%").arg(m_nScale));
    }

    setScaleRotateViewModeAndShow();

//    AppSetting::instance()->setKeyValue(KEY_PERCENTAGE, "100");
//    AppSetting::instance()->setKeyValue(KEY_ADAPTAT, "0");
    DataManager::instance()->setFontScale(QString::number(100));
    DataManager::instance()->setFontFit(QString::number(0));
}

/**
 * @brief FontMenu::setFileViewScale
 * 键盘快捷键控制文档视图缩放
 * @param large true:放大   false:缩小
 */
void FontMenu::setFileViewScale(bool larger)
{
    m_bIsAdaptMove = false;
    if (larger) {
        int scale = m_pEnlargeSlider->value();
        if (scale < 500) {
            scale += 25;
            if (scale > 500) {
                scale = 500;
            }
            m_pEnlargeSlider->setValue(scale);
        }
    } else {
        int scale = m_pEnlargeSlider->value();
        if (scale > 10) {
            scale -= 25;
            if (scale <= 10) {
                scale = 10;
            }
            m_pEnlargeSlider->setValue(scale);
        }
    }
}

/**
 * @brief FontMenu::setSliderMaxValue
 * 打开文件时，设置缩放最大值，不一定是500%
 */
void FontMenu::setSliderMaxValue()
{
    int maxZoomRatio = 100;
    maxZoomRatio = static_cast<int>(DocummentProxy::instance()->getMaxZoomratio() * 100);

    if (DocummentProxy::instance() && m_pEnlargeSlider->maximum() != maxZoomRatio) {
        int maxVal = 500;
        maxVal = static_cast<int>(DocummentProxy::instance()->getMaxZoomratio() * 100);
        m_pEnlargeSlider->setMaximum(maxVal);
    }
}

/**
 * @brief FontMenu::slotTwoPage
 * 双页显示
 */
void FontMenu::slotTwoPage()
{
    m_bDoubPage = !m_bDoubPage;

    scaleAndRotate();
    if (m_bFiteW) {
        notifyMsg(MSG_SELF_ADAPTE_WIDTH, QString::number(1));  //emit sigFiteW(QString::number(1));
    } else if (m_bFiteH) {
        notifyMsg(MSG_SELF_ADAPTE_HEIGHT, QString::number(1));  //emit sigFiteH(QString::number(1));
    }

//    AppSetting::instance()->setKeyValue(KEY_DOUBPAGE, QString::number(m_bDoubPage));
    DataManager::instance()->setFontDoubPage(QString::number(m_bDoubPage));
}

/**
 * @brief FontMenu::slotFiteH
 *自适应高
 */
void FontMenu::slotFiteH()
{
    m_pFiteWAction->setChecked(false);
    m_bFiteW = false;
    m_bFiteH = !m_bFiteH;
    m_pFiteHAction->setChecked(m_bFiteH);

    setAppSetFiteHAndW();
    int t_nShow = m_bFiteH ? 1 : 0;

    notifyMsg(MSG_SELF_ADAPTE_HEIGHT, QString::number(t_nShow));  //emit sigFiteH(QString::number(t_nShow));

//    AppSetting::instance()->setKeyValue(KEY_ADAPTAT, QString::number(m_bFiteH ? 10 : (m_bFiteW ? 1 : 0)));
    DataManager::instance()->setFontFit(QString::number(m_bFiteH ? 10 : (m_bFiteW ? 1 : 0)));
}

/**
 * @brief FontMenu::slotFiteW
 * 自适应宽
 */
void FontMenu::slotFiteW()
{
    m_pFiteHAction->setChecked(false);
    m_bFiteH = false;
    m_bFiteW = !m_bFiteW;
    m_pFiteWAction->setChecked(m_bFiteW);

    setAppSetFiteHAndW();
    int t_nShow = m_bFiteW ? 1 : 0;

    notifyMsg(MSG_SELF_ADAPTE_WIDTH, QString::number(t_nShow));  //emit sigFiteW(QString::number(t_nShow));

//    AppSetting::instance()->setKeyValue(KEY_ADAPTAT, QString::number(m_bFiteH ? 10 : (m_bFiteW ? 1 : 0)));
    DataManager::instance()->setFontFit(QString::number(m_bFiteH ? 10 : (m_bFiteW ? 1 : 0)));
}

/**
 * @brief FontMenu::slotRotateL
 * 左旋转
 */
void FontMenu::slotRotateL()
{
    rotateThumbnail(false);
}

/**
 * @brief FontMenu::slotRotateR
 * 右旋转
 */
void FontMenu::slotRotateR()
{
    rotateThumbnail(true);
}

/**
 * @brief FontMenu::slotScaleValChanged
 * 缩放比例变化
 */
void FontMenu::slotScaleValChanged(int scale)
{
    m_nScale = scale;

    if (m_pEnlargeLab) {
        m_pEnlargeLab->clear();
        m_pEnlargeLab->setText(QString("%1%").arg(scale));
    }

    if (!m_bIsAdaptMove) {
        resetFiteHAndW();
        scaleAndRotate();
    } else {
//        AppSetting::instance()->setKeyValue(KEY_PERCENTAGE, QString::number(m_nScale));
        DataManager::instance()->setFontScale(QString::number(m_nScale));
    }

    m_bIsAdaptMove = false;
}

/**
 * @brief FontMenu::slotFileOpenOk
 * 打开文件，加载参数
 */
void FontMenu::slotFileOpenOk()
{
    setSliderMaxValue();

    int value = 0;

    //缩放比例
    value = DataManager::instance()->getFontScale().toInt();//AppSetting::instance()->getKeyValue(KEY_PERCENTAGE).toInt();
    if (value > 0) {
        m_bIsAdaptMove = true;
        m_pEnlargeSlider->setValue(value);
        m_bIsAdaptMove = false;
    }

    //单双页
    value = DataManager::instance()->getFontDoubPage().toInt();//AppSetting::instance()->getKeyValue(KEY_DOUBPAGE).toInt();
    m_bDoubPage = (value == 1) ? true : false;
    if (m_pTwoPageAction) {
        m_pTwoPageAction->setChecked(m_bDoubPage);
    }

    //自适应宽/高
    int adaptat = DataManager::instance()->getFontFit().toInt();//AppSetting::instance()->getKeyValue(KEY_ADAPTAT).toInt();
    if (adaptat == 1) {
        m_bFiteW = true;
        m_bFiteH = false;
        m_pFiteWAction->setChecked(m_bFiteW);
        m_pFiteHAction->setChecked(m_bFiteH);
//        notifyMsg(MSG_SELF_ADAPTE_WIDTH, QString::number(1));          //emit sigFiteW(QString::number(1));
    } else if (adaptat == 10) {
        m_bFiteH = true;
        m_bFiteW = false;
        m_pFiteWAction->setChecked(m_bFiteW);
        m_pFiteHAction->setChecked(m_bFiteH);
//        notifyMsg(MSG_SELF_ADAPTE_HEIGHT, QString::number(1));          //emit sigFiteH(QString::number(1));
    }

    //旋转度数
    m_nRotate = DataManager::instance()->getFontRotate().toInt();//AppSetting::instance()->getKeyValue(KEY_ROTATE).toInt();
    m_nRotate %= 360;

//    scaleAndRotate();
}

/**
 * @brief FontMenu::slotDealWithShortKey
 * 处理键盘事件
 * @param keyType  事件内容
 */
void FontMenu::slotDealWithShortKey(const QString &keyType)
{
    if (keyType == KeyStr::g_ctrl_1) {
        //还原
        resetAdaptive();
    } else if (keyType == KeyStr::g_ctrl_2) {
        //自适应高
        slotFiteH();
    } else if (keyType == KeyStr::g_ctrl_3) {
        //自适应宽
        slotFiteW();
    } else if (keyType == KeyStr::g_ctrl_r) {
        //左旋转
        rotateThumbnail(false);
    } else if (keyType == KeyStr::g_ctrl_shift_r) {
        //右旋转
        rotateThumbnail(true);
    } else if (keyType == KeyStr::g_ctrl_smaller) {
        //缩放
        setFileViewScale(false);
    } else if (keyType == KeyStr::g_ctrl_larger || keyType == KeyStr::g_ctrl_equal) {
        //放大
        setFileViewScale(true);
    } else {
        return;
    }
}

/**
 * @brief FontMenu::slotSetCurScale
 * 根据自适应宽高设置缩放比例scale
 * @param scale   缩放比例
 */
void FontMenu::slotSetCurScale(const QString &scale)
{
//    setScaleVal(static_cast<int>(scale.toDouble() * 100));
    int nScale = 100;

    nScale = static_cast<int>(scale.toDouble() * 100);

    m_bIsAdaptMove = true;
    //设置 当前的缩放比, 不要触发滑动条值变化信号
    //m_pEnlargeSlider->blockSignals(true);
    m_pEnlargeSlider->setValue(nScale);
    m_bIsAdaptMove = false;
    //m_pEnlargeSlider->blockSignals(false);
}

/**
 * @brief FontMenu::initMenu
 * 初始化Menu
 */
void FontMenu::initMenu()
{
    initScale();

    this->addSeparator();

    //双页
    m_pTwoPageAction = createAction(tr("Two-Page View"), SLOT(slotTwoPage()), true);

    //自适应高
    m_pFiteHAction = createAction(tr("Fit Height"), SLOT(slotFiteH()), true);

    //自适应宽
    m_pFiteWAction = createAction(tr("Fit Width"), SLOT(slotFiteW()), true);

    //左旋转
    m_pRotateLAction = createAction(tr("Rotate Left"), SLOT(slotRotateL()));

    //右旋转
    m_pRotateRAction = createAction(tr("Rotate Right"), SLOT(slotRotateR()));

    //设置菜单中字体大小
    DFontSizeManager::instance()->bind(this, DFontSizeManager::T6);
}

/**
 * @brief FontMenu::initScale
 * 初始化缩放比例
 */
void FontMenu::initScale()
{
    DWidget *scaleWidget = new DWidget(this);
    if (scaleWidget) {
        scaleWidget->setMinimumWidth(220);
        scaleWidget->setMinimumHeight(80);
        QHBoxLayout     *t_pEnlargeLabLayout = new QHBoxLayout;
        t_pEnlargeLabLayout->setContentsMargins(0, 0, 0, 0);
        QHBoxLayout     *t_pEnlargeSliderLayout = new QHBoxLayout;
        t_pEnlargeSliderLayout->setContentsMargins(23, 0, 23, 0);
        QVBoxLayout     *t_pWidgetLayout = new QVBoxLayout;
        t_pWidgetLayout->setContentsMargins(0, 0, 0, 0);

        m_pEnlargeLab = new DLabel(scaleWidget);
        m_pEnlargeLab->setText("100%");
        m_pEnlargeLab->setAlignment(Qt::AlignCenter);
        m_pEnlargeLab->setForegroundRole(DPalette::BrightText);
        DFontSizeManager::instance()->bind(m_pEnlargeLab, DFontSizeManager::T5);

        t_pEnlargeLabLayout->addStretch(1);
        t_pEnlargeLabLayout->addWidget(m_pEnlargeLab);
        t_pEnlargeLabLayout->addStretch(1);

        m_pEnlargeSlider = new DSlider(Qt::Horizontal, scaleWidget);
        QIcon icon;
        icon = PF::getIcon(Pri::g_module + "a_small");
        m_pEnlargeSlider->setLeftIcon(icon);
        icon = PF::getIcon(Pri::g_module + "a_big");
        m_pEnlargeSlider->setRightIcon(icon);
        t_pEnlargeSliderLayout->addWidget(m_pEnlargeSlider);
        t_pEnlargeSliderLayout->setAlignment(Qt::AlignCenter);
        m_pEnlargeSlider->setMinimum(10);
        m_pEnlargeSlider->setMaximum(500);
        m_pEnlargeSlider->setValue(100);
        m_pEnlargeSlider->slider()->setSingleStep(25);
        m_pEnlargeSlider->setPageStep(25);
        connect(m_pEnlargeSlider, SIGNAL(valueChanged(int)), this, SLOT(slotScaleValChanged(int)));

        t_pWidgetLayout->addItem(t_pEnlargeLabLayout);
        t_pWidgetLayout->addItem(t_pEnlargeSliderLayout);
        scaleWidget->setLayout(t_pWidgetLayout);

        QWidgetAction *widgetAction = new QWidgetAction(this);
        widgetAction->setDefaultWidget(scaleWidget);
        this->addAction(widgetAction);
    }
}

/**
 * @brief FontMenu::initConnection
 * 初始化信号槽
 */
void FontMenu::initConnection()
{
    connect(this, SIGNAL(sigFileOpenOk()), this, SLOT(slotFileOpenOk()));
    connect(this, SIGNAL(sigDealWithShortKey(const QString &))
            , this, SLOT(slotDealWithShortKey(const QString &)));
    connect(this, SIGNAL(sigSetCurScale(const QString &)), this, SLOT(slotSetCurScale(const QString &)));
}

/**
 * @brief FontMenu::createAction   创建菜单action
 * @param objName                  action名称
 * @param member                   action所关联的响应函数
 * @param checked                  action是否有选中状态
 * @return                         当前action指针
 */
QAction *FontMenu::createAction(const QString &objName, const char *member, bool checked)
{
    auto action = new QAction(objName, this);

    if (action) {
        action->setObjectName(objName);
        action->setCheckable(checked);

        connect(action, SIGNAL(triggered()), this, member);

        this->addAction(action);
        this->addSeparator();

        return action;
    }
    return nullptr;
}

/**
 * @brief FontMenu::rotateThumbnail 旋转文档和缩略图
 * @param direct true:向右 false:向左
 */
void FontMenu::rotateThumbnail(bool direct)
{
    if (direct) {
        m_nRotate += 90;
    } else {
        m_nRotate -= 90;
    }

    m_nRotate = (m_nRotate < 0) ? (m_nRotate + 360) : (m_nRotate % 360);

    calcRotateType();

    setScaleRotateViewModeAndShow();

//    qDebug() << __FUNCTION__ << "  m_nRotate:" << m_nRotate;

//    AppSetting::instance()->setKeyValue(KEY_ROTATE, QString::number(m_nRotate));
    DataManager::instance()->setFontRotate(QString::number(m_nRotate));

    notifyMsg(MSG_FILE_ROTATE, QString::number(m_nRotate));  //emit sigRotate(QString::number(m_nRotate));
}

/**
 * @brief FontMenu::scaleAndRotate
 * 缩放和旋转
 */
void FontMenu::scaleAndRotate()
{
    qDebug() << __FUNCTION__ << "      12334566478699     ";
    calcRotateType();

    setScaleRotateViewModeAndShow();

//    AppSetting::instance()->setKeyValue(KEY_DOUBPAGE, QString::number(m_bDoubPage));
//    AppSetting::instance()->setKeyValue(KEY_PERCENTAGE, QString::number(m_nScale));
//    AppSetting::instance()->setKeyValue(KEY_ADAPTAT, QString::number(m_bFiteH ? 10 : (m_bFiteW ? 1 : 0)));
//    AppSetting::instance()->setKeyValue(KEY_ROTATE, QString::number(m_nRotate));

    DataManager::instance()->setFontScale(QString::number(m_nScale));
    DataManager::instance()->setFontDoubPage(QString::number(m_bDoubPage));
    DataManager::instance()->setFontFit(QString::number(m_bFiteH ? 10 : (m_bFiteW ? 1 : 0)));
    DataManager::instance()->setFontRotate(QString::number(m_nRotate));
}

/**
 * @brief FontMenu::calcRotateType
 * 计算旋转类型(后台所需)
 */
void FontMenu::calcRotateType()
{
    int t_rotate = 0;

    t_rotate = (m_nRotate / 90) % 4;

    t_rotate = (t_rotate < 0) ? (t_rotate * -1) : t_rotate;

    t_rotate += 1;

    switch (t_rotate) {
    case RotateType_0:
        m_nRotate = 0;
        m_rotateType = RotateType_0;
        break;
    case RotateType_90:
        m_rotateType = RotateType_90;
        break;
    case RotateType_180:
        m_rotateType = RotateType_180;
        break;
    case RotateType_270:
        m_rotateType = RotateType_270;
        break;
    default:
        m_rotateType = RotateType_Normal;
        break;
    }
}

/**
 * @brief FontMenu::setScaleRotateViewModeAndShow
 * 调用文件解析库接口，设置文档的缩放、缩放、视图
 */
void FontMenu::setScaleRotateViewModeAndShow()
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {

        double scale = (m_nScale * 0.01);
        ViewMode_EM viewmode = ViewMode_SinglePage;
        if (m_bDoubPage) {
            viewmode = ViewMode_FacingPage;
        }

        _proxy->setScaleRotateViewModeAndShow(scale, m_rotateType, viewmode);
    }
}

/**
 * @brief FontMenu::setAppSetFiteHAndW
 * 记录文档属性,下次加载时使用
 * 0:都不自适应  1:自适应宽  10:自适应高
 */
void FontMenu::setAppSetFiteHAndW()
{
    int iValue = 0;

    if (m_bFiteW) {
        iValue = 1;
    } else if (m_bFiteH) {
        iValue = 10;
    }

//    AppSetting::instance()->setKeyValue(KEY_ADAPTAT, QString::number(iValue));
    DataManager::instance()->setFontFit(QString::number(m_bFiteH ? 10 : (m_bFiteW ? 1 : 0)));
}

/**
 * @brief FontMenu::resetFiteHAndW
 * 手动改变缩放比例，复位自适应宽高
 */
void FontMenu::resetFiteHAndW()
{
    //缩放比例, 取消 自适应宽/高
    if (m_bFiteW) {
        notifyMsg(MSG_SELF_ADAPTE_WIDTH, QString::number(0));  //emit sigFiteW(QString::number(0));
    } else if (m_bFiteH) {
        notifyMsg(MSG_SELF_ADAPTE_HEIGHT, QString::number(0));  //emit sigFiteH(QString::number(0));
    }

    m_bFiteH = false;
    m_bFiteW = false;
    if (m_pFiteHAction) {
        m_pFiteHAction->setChecked(m_bFiteH);
    }
    if (m_pFiteWAction) {
        m_pFiteWAction->setChecked(m_bFiteW);
    }
}
