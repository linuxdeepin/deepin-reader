// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef EYE_PROTECTION_ACTION_H
#define EYE_PROTECTION_ACTION_H

#include <QWidgetAction>

#include <DWidget>

DWIDGET_USE_NAMESPACE

class RoundColorWidget;
class EyeProtectionManager;

/**
 * @brief The EyeProtectionAction class
 * 护眼模式菜单项（自定义 QWidgetAction）
 * 内联展示在主菜单中，使用 RoundColorWidget 圆形色块单选
 *   第一行：标题 "阅读模式"
 *   第二行：4个圆形色块并排，圆形填充对应背景色
 * 选中项外围显示指示环（系统激活色）
 */
class EyeProtectionAction : public QWidgetAction
{
    Q_OBJECT
    Q_DISABLE_COPY(EyeProtectionAction)

public:
    explicit EyeProtectionAction(DWidget *parent = nullptr);

private slots:
    /**
     * @brief onModeChanged 响应 EyeProtectionManager 的模式变化，同步选中态
     * @param mode 新模式
     */
    void onModeChanged(int mode);

    /**
     * @brief onBtnClicked 圆形控件点击
     * @param index 模式索引
     */
    void onBtnClicked(int index);

private:
    /**
     * @brief initWidget 初始化控件布局
     * @param parent 父控件
     */
    void initWidget(DWidget *parent);

    RoundColorWidget *m_buttons[4] = {};
};

#endif // EYE_PROTECTION_ACTION_H
