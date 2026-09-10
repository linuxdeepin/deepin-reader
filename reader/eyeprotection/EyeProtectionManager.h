// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef EYE_PROTECTION_MANAGER_H
#define EYE_PROTECTION_MANAGER_H

#include <QObject>
#include <QColor>

/**
 * @brief The EyeProtectionManager class
 * 护眼模式管理器（单例）
 * 提供四种阅读模式：无护眼、经典护眼、绿色护眼、夜间护眼
 * 通过 modeChanged 信号通知视图层更新背景色
 */
class EyeProtectionManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Mode 护眼模式枚举
     */
    enum Mode {
        Off = 0,            // 无护眼（默认）
        Classic = 1,        // 经典护眼（米黄）
        Green = 2,          // 绿色护眼
        Night = 3            // 夜间护眼（深灰）
    };
    Q_ENUM(Mode)

    /**
     * @brief NightImagePolicy 夜间模式图片对象处理策略(实验)
     * 图片对象区域不做反色以免连续色调负片失真,按此策略处理
     */
    enum NightImagePolicy {
        NightImageDim = 0,         // 调暗(默认,系数见 nightImageDimFactor)
        NightImageInvert = 1,      // 也反色(旧版行为)
        NightImageOriginal = 2     // 保持原图
    };
    Q_ENUM(NightImagePolicy)

    static EyeProtectionManager *instance();

    /**
     * @brief setMode 设置护眼模式
     * @param mode 模式
     */
    void setMode(Mode mode);

    /**
     * @brief mode 获取当前护眼模式
     * @return 当前模式
     */
    Mode mode() const;

    /**
     * @brief nightImagePolicy 夜间模式图片对象处理策略
     * 可被环境变量 DEEPIN_READER_NIGHT_IMAGE_POLICY(dim|invert|original)覆盖,便于实验
     */
    NightImagePolicy nightImagePolicy() const;

    /**
     * @brief nightImageDimFactor 图片调暗系数(0..1,默认 1.0 = 保持原亮度)
     * 可被环境变量 DEEPIN_READER_NIGHT_IMAGE_DIM(如 0.6)覆盖
     */
    qreal nightImageDimFactor() const;

    /**
     * @brief pageBackgroundColor 获取文档页面背景色（也是圆形控件填充色）
     * @return 页面背景色
     */
    QColor pageBackgroundColor() const;

    /**
     * @brief viewportBackgroundColor 获取视口空白区背景色（比页面更深）
     * @return 视口背景色
     */
    QColor viewportBackgroundColor() const;

    /**
     * @brief foregroundColor 获取前景文字色
     * @return 文字色
     */
    QColor foregroundColor() const;

    /**
     * @brief pageBackgroundColor 获取指定模式的页面背景色（静态，不依赖当前模式）
     * @param mode 模式
     * @return 页面背景色
     */
    static QColor pageBackgroundColor(Mode mode);

    /**
     * @brief viewportBackgroundColor 获取指定模式的视口背景色（静态，不依赖当前模式）
     * @param mode 模式
     * @return 视口背景色
     */
    static QColor viewportBackgroundColor(Mode mode);

signals:
    /**
     * @brief modeChanged 模式切换信号
     * @param mode 新模式
     */
    void modeChanged(Mode mode);

    /**
     * @brief nightImagePolicyChanged 夜间图片策略变化信号
     */
    void nightImagePolicyChanged();

private:
    Q_DISABLE_COPY(EyeProtectionManager)

    explicit EyeProtectionManager(QObject *parent = nullptr);

    /**
     * @brief loadMode 从配置加载模式
     */
    void loadMode();

    /**
     * @brief saveMode 保存模式到配置
     */
    void saveMode();

    Mode m_mode = Off;
    NightImagePolicy m_nightImagePolicy = NightImageDim;
    qreal m_nightImageDimFactor = 1.0;   // 与 loadMode 默认一致,避免 loadMode 前读取到旧值

    // 各模式颜色映射
    struct ModeColors {
        QColor pageBg;          // 页面背景色
        QColor viewportBg;      // 视口空白区背景色
        QColor foreground;      // 前景色
    };

    static const ModeColors s_modeColors[4];
};

#endif // EYE_PROTECTION_MANAGER_H
