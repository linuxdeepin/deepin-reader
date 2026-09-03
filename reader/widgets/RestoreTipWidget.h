// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RESTORETIPWIDGET_H
#define RESTORETIPWIDGET_H

#include <DWidget>
#include <DPushButton>
#include <DIconButton>
#include <DDciIcon>

DWIDGET_USE_NAMESPACE

class QLabel;

/**
 * @brief The RestoreTipWidget class
 * 恢复阅读位置提示条
 * 布局：[TIP图标] [文字] [跳转到首页按钮] [关闭按钮]
 * 位于页面底部居中，用户手动关闭
 */
class RestoreTipWidget : public DWidget
{
    Q_OBJECT

public:
    explicit RestoreTipWidget(QWidget *parent = nullptr);

    /**
     * @brief 显示提示条
     */
    void showTip();

    /**
     * @brief adjustSize 根据字体自适应控件高度
     */
    void adjustSize();

    /**
     * @brief reposition 重新定位到底部居中
     */
    void reposition();

signals:
    /**
     * @brief sigJumpToFirstPage 用户点击"跳转到首页"，请求跳转
     */
    void sigJumpToFirstPage();

    /**
     * @brief sigCloseRestoreTip 用户点击"关闭"，请求关闭提示条并清除恢复标记
     *
     * 关闭后本次打开文档期间不再显示，切换标签页再切回不会重复出现；
     * 下次打开文档时 DocSheet::onOpened 会重新置位，仍会提示。
     */
    void sigCloseRestoreTip();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    /**
     * @brief initUI 初始化UI布局
     */
    void initUI();

    /**
     * @brief onUpdateTheme 更新主题相关颜色
     */
    void onUpdateTheme();

    /**
     * @brief refreshTipIcon 根据当前主题刷新 TIP 图标像素图
     */
    void refreshTipIcon();

private slots:
    /**
     * @brief onFontChanged 系统字体变化时重新自适应
     */
    void onFontChanged();

private:
    QString m_text;
    QColor m_backgroundColor;
    QColor m_textColor;

    DPushButton *m_jumpBtn = nullptr;
    DIconButton *m_closeBtn = nullptr;
    QLabel *m_iconLabel = nullptr;
    DDciIcon m_alertIcon;

    int m_lrMargin = 12;   // 左右内边距
    int m_tbMargin = 6;    // 上下内边距
    int m_spacing = 8;     // 控件间距
};

#endif // RESTORETIPWIDGET_H
