// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NIGHTFILTER_H
#define NIGHTFILTER_H

#include <QImage>
#include <QVector>
#include <QRectF>

/**
 * @brief The NightFilter namespace
 * 夜间模式智能滤镜(实验):
 *   - 文字/矢量区域:CIELAB L* 反转(感知均匀的明度反转,保留色度 a/b)
 *   - 图片对象区域:不反色,按策略调暗/原图(避免连续色调照片负片失真)
 *
 * 通过"对象蒙版"分区,实现"文字反色、图片不失真"。
 * 蒙版由渲染层根据 PDF 页面对象(IMAGE 类型 bbox)生成,
 * 与渲染输出像素一一对齐;Grayscale8 格式,255 = 图片对象区域。
 *
 * 设计调研见 docs/night-mode-image-invert-research.md
 */
namespace NightFilter {

/**
 * @brief 图片对象区域的处理策略
 */
enum ImagePolicy {
    ImageDim = 0,       // 图片调暗(默认,业界共识做法)
    ImageInvert = 1,    // 图片也反色(扫描文档场景)
    ImageOriginal = 2   // 图片保持原图
};

struct Options {
    ImagePolicy imagePolicy = ImageDim;
    qreal imageDimFactor = 1.0;      // 图片调暗系数(0..1),1=保持原亮度(仅受整页暗罩影响)
    qreal minLightness = 11.0;       // 反转后 L* 下限(约 #1E1E1E,防整页全黑刺眼)
    qreal boostSourceBelow = 27.0;   // 原 L* 低于此值(约 #404040)的像素反转后提纯白,保文字对比度
};

/**
 * @brief apply 对 src 应用夜间滤镜
 * @param src 渲染输出(ARGB32 / RGB32)
 * @param mask 对象蒙版,可为 nullptr(整图反色,即旧版行为增强)
 * @param opt 滤镜参数
 * @return 处理后的图像(格式与输入一致)
 */
QImage apply(const QImage &src, const QImage *mask = nullptr, const Options &opt = Options());

/**
 * @brief applyPage 页级夜间滤镜入口(可在非 UI 线程调用,无锁)
 *
 * 内部完成:扫描页特判(bbox 覆盖率 > kScannedCoverThreshold 时整页反色)、
 * 对象蒙版构建(含 2px 膨胀)与分区处理。
 * @param imageRects 图片对象 bbox 列表,物理像素坐标,与 src 一一对齐;可为空(整页反色)
 * @param coverageOut 可选,回传图片对象覆盖率(诊断/日志用)
 */
QImage applyPage(const QImage &src, const QVector<QRectF> &imageRects,
                 const Options &opt = Options(), double *coverageOut = nullptr);

/** 扫描页特判阈值:图片对象 bbox 覆盖率超过此值视为扫描件,回退整页反色 */
constexpr double kScannedCoverThreshold = 0.7;

/** sRGB(0..255)→ CIELAB(D65),单像素 */
void srgbToLab(int r, int g, int b, double &L, double &a, double &bb);

/** CIELAB(D65)→ sRGB(0..255),越界自动钳制 */
void labToSrgb(double L, double a, double bb, int &r, int &g, int &b);

} // namespace NightFilter

#endif // NIGHTFILTER_H
