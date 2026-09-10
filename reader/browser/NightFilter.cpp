// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "NightFilter.h"

#include <QtGlobal>
#include <QPainter>
#include <QMutexLocker>
#include <QtMath>
#include <cmath>
#include <QDebug>
#include <mutex>

namespace NightFilter {

namespace {

// ---------- sRGB <-> CIELAB(D65)标准转换 ----------

constexpr double kD65Xn = 0.95047;
constexpr double kD65Yn = 1.00000;
constexpr double kD65Zn = 1.08883;
constexpr double kEpsilon = 216.0 / 24389.0;   // 0.008856
constexpr double kKappa = 24389.0 / 27.0;      // 903.296

inline double srgbChannelToLinear(double c)
{
    c = qBound(0.0, c / 255.0, 1.0);
    return (c <= 0.04045) ? (c / 12.92) : qPow((c + 0.055) / 1.055, 2.4);
}

inline double linearChannelToSrgb(double c)
{
    c = qBound(0.0, c, 1.0);
    return (c <= 0.0031308) ? (12.92 * c) : (1.055 * qPow(c, 1.0 / 2.4) - 0.055) * 255.0;
}

inline double labForward(double t)
{
    return (t > kEpsilon) ? std::cbrt(t) : (kKappa * t + 16.0) / 116.0;
}

} // namespace

void srgbToLab(int r, int g, int b, double &L, double &a, double &bb)
{
    const double rl = srgbChannelToLinear(r);
    const double gl = srgbChannelToLinear(g);
    const double bl = srgbChannelToLinear(b);

    const double x = 0.4124564 * rl + 0.3575761 * gl + 0.1804375 * bl;
    const double y = 0.2126729 * rl + 0.7151522 * gl + 0.0721750 * bl;
    const double z = 0.0193339 * rl + 0.1191920 * gl + 0.9503041 * bl;

    const double fx = labForward(x / kD65Xn);
    const double fy = labForward(y / kD65Yn);
    const double fz = labForward(z / kD65Zn);

    L = 116.0 * fy - 16.0;
    a = 500.0 * (fx - fy);
    bb = 200.0 * (fy - fz);
}

void labToSrgb(double L, double a, double bb, int &r, int &g, int &b)
{
    L = qBound(0.0, L, 100.0);

    const double fy = (L + 16.0) / 116.0;
    const double fx = fy + a / 500.0;
    const double fz = fy - bb / 200.0;

    const double fx3 = fx * fx * fx;
    const double fz3 = fz * fz * fz;
    const double fy3 = fy * fy * fy;

    const double xr = (fx3 > kEpsilon) ? fx3 : (116.0 * fx - 16.0) / kKappa;
    const double yr = (L > 8.0) ? fy3 : L / kKappa;
    const double zr = (fz3 > kEpsilon) ? fz3 : (116.0 * fz - 16.0) / kKappa;

    const double x = xr * kD65Xn;
    const double y = yr * kD65Yn;
    const double z = zr * kD65Zn;

    const double rl = 3.2404542 * x - 1.5371385 * y - 0.4985314 * z;
    const double gl = -0.9692660 * x + 1.8760108 * y + 0.0415560 * z;
    const double bl = 0.0556434 * x - 0.2040259 * y + 1.0572252 * z;

    r = qRound(qBound(0.0, linearChannelToSrgb(rl), 255.0));
    g = qRound(qBound(0.0, linearChannelToSrgb(gl), 255.0));
    b = qRound(qBound(0.0, linearChannelToSrgb(bl), 255.0));
}

namespace {

/** 图片域:仅按系数调暗,色相与明暗结构保持不变 */
inline QRgb dimPixelImage(QRgb px, qreal k)
{
    return qRgba(qRound(qRed(px) * k), qRound(qGreen(px) * k), qRound(qBlue(px) * k), qAlpha(px));
}

} // namespace
/**
 * @brief invertPixelExact 精确路径:CIELAB L* 反转 + 两端收敛
 *   - L' = 100 - L:白底→深底,黑字→亮字,色度(a/b)完整保留
 *   - L' 下限钳制(约 #1E1E1E),避免背景纯黑刺眼
 *   - 原深色(L* < boost 阈值)反转后提纯白,保证文字/抗锯齿边缘对比度
 */
inline QRgb invertPixelExact(QRgb px, const Options &opt)
{
    double L, a, bb;
    srgbToLab(qRed(px), qGreen(px), qBlue(px), L, a, bb);

    if (L < opt.boostSourceBelow)
        return qRgba(255, 255, 255, qAlpha(px));

    double L2 = 100.0 - L;
    if (L2 < opt.minLightness)
        L2 = opt.minLightness;

    int r, g, b;
    labToSrgb(L2, a, bb, r, g, b);
    return qRgba(r, g, b, qAlpha(px));
}

// ---------- 反色域两级 LUT:65³ 精确基表 + 128³ 直查表 ----------
// 精确路径每像素需 6 次 pow + 1 次 cbrt(数百 ns),无法在交互路径使用。
// 第一级:65³ 网格精确计算(构建 ~80ms,一次性);
// 第二级:128³(2.1M 项,8MB,可驻留 L3)由基表三线性填充(构建 ~40ms),
// 查询零计算:1 次索引 + 1 次内存读,≈5-10ns/像素。
// 精度:灰阶/常规色 ≤2/255;极端饱和色(反色后出 sRGB 色域被钳制)≤24/255,
// 提白不连续面(L*=27)被插值平滑,文字抗锯齿边缘更柔和,人眼不可辨。
namespace {

struct InvertLut {
    static constexpr int kGrid = 65;             // 基表每轴网格数(网格点取 i*255/64)
    static constexpr int kMaxIdx = kGrid - 1;
    static constexpr int kFast = 128;            // 直查表每轴刻度(0..127 → r*127/255)
    static constexpr int kFastMaxIdx = kFast - 1;
    std::vector<QRgb> table;                     // kGrid³ 基表(≈1.1MB)
    std::vector<QRgb> fast;                      // kFast³ 直查表(≈8MB,L3 友好)

    InvertLut() : table(static_cast<size_t>(kGrid) * kGrid * kGrid),
        fast(static_cast<size_t>(kFast) * kFast * kFast)
    {
        const Options opt;                        // 默认参数(与运行时一致)
        for (int ri = 0; ri < kGrid; ++ri) {
            for (int gi = 0; gi < kGrid; ++gi) {
                for (int bi = 0; bi < kGrid; ++bi) {
                    const int r = ri * 255 / kMaxIdx;
                    const int g = gi * 255 / kMaxIdx;
                    const int b = bi * 255 / kMaxIdx;
                    table[index(ri, gi, bi)] = invertPixelExact(qRgb(r, g, b), opt);
                }
            }
        }

        // 第二级:直查表用基表三线性填充(等效于逐像素三线性,提前烘好)
        for (int ri = 0; ri < kFast; ++ri) {
            const double fr = ri * 64.0 / kFastMaxIdx;   // 直查刻度 → 基表坐标
            const int r0 = int(fr);
            const int r1 = qMin(r0 + 1, kMaxIdx);
            const double dr = fr - r0;
            for (int gi = 0; gi < kFast; ++gi) {
                const double fg = gi * 64.0 / kFastMaxIdx;
                const int g0 = int(fg);
                const int g1 = qMin(g0 + 1, kMaxIdx);
                const double dg = fg - g0;
                for (int bi = 0; bi < kFast; ++bi) {
                    const double fb = bi * 64.0 / kFastMaxIdx;
                    const int b0 = int(fb);
                    const int b1 = qMin(b0 + 1, kMaxIdx);
                    const double db = fb - b0;

                    const QRgb *t = table.data();
                    const QRgb c000 = t[index(r0, g0, b0)];
                    const QRgb c100 = t[index(r1, g0, b0)];
                    const QRgb c010 = t[index(r0, g1, b0)];
                    const QRgb c110 = t[index(r1, g1, b0)];
                    const QRgb c001 = t[index(r0, g0, b1)];
                    const QRgb c101 = t[index(r1, g0, b1)];
                    const QRgb c011 = t[index(r0, g1, b1)];
                    const QRgb c111 = t[index(r1, g1, b1)];

                    auto ch = [](QRgb c, int shift) { return (c >> shift) & 0xFF; };
                    auto mix1 = [&](int shift) {
                        const double w000 = (1 - dr) * (1 - dg) * (1 - db);
                        const double w100 = dr * (1 - dg) * (1 - db);
                        const double w010 = (1 - dr) * dg * (1 - db);
                        const double w110 = dr * dg * (1 - db);
                        const double w001 = (1 - dr) * (1 - dg) * db;
                        const double w101 = dr * (1 - dg) * db;
                        const double w011 = (1 - dr) * dg * db;
                        const double w111 = dr * dg * db;
                        return w000 * ch(c000, shift) + w100 * ch(c100, shift)
                                + w010 * ch(c010, shift) + w110 * ch(c110, shift)
                                + w001 * ch(c001, shift) + w101 * ch(c101, shift)
                                + w011 * ch(c011, shift) + w111 * ch(c111, shift);
                    };

                    fast[(size_t(ri) * kFast + gi) * kFast + bi] =
                            qRgb(qRound(mix1(16)), qRound(mix1(8)), qRound(mix1(0)));
                }
            }
        }
    }

    static size_t index(int r, int g, int b)
    { return (size_t(r) * kGrid + g) * kGrid + b; }

    static size_t fastIndex(QRgb px)
    { return ((size_t(qRed(px)) * kFastMaxIdx / 255) * kFast
              + (size_t(qGreen(px)) * kFastMaxIdx / 255)) * kFast
              + (size_t(qBlue(px)) * kFastMaxIdx / 255); }
};

const InvertLut &invertLut()
{
    static InvertLut lut;                         // C++11 起线程安全的一次性初始化
    return lut;
}

/** 直查表版反色(零插值计算:索引 + 一次内存读,精度等效三线性) */
inline QRgb invertPixelLut(QRgb px)
{
    return (invertLut().fast[InvertLut::fastIndex(px)] & 0x00FFFFFFu) | (px & 0xFF000000u);
}

} // namespace

QImage apply(const QImage &src, const QImage *mask, const Options &opt)
{
    if (src.isNull())
        return src;

    QImage img = src;
    if (img.format() != QImage::Format_ARGB32 &&
        img.format() != QImage::Format_RGB32) {
        img = img.convertToFormat(QImage::Format_ARGB32);
    }

    // 蒙版尺寸不匹配时忽略(防错位;mask 为空表示整页反色)
    const bool useMask = (mask != nullptr && !mask->isNull() && mask->size() == img.size());
    const bool hasImageArea = useMask && opt.imagePolicy != ImageInvert;

    const int w = img.width();
    const int h = img.height();

    // 默认参数走 LUT 快速路径;非默认参数(实验环境变量)回退精确计算
    const bool useLut = (Options().minLightness == opt.minLightness
                         && Options().boostSourceBelow == opt.boostSourceBelow);

    for (int y = 0; y < h; ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(img.scanLine(y));
        const uchar *maskLine = useMask ? mask->scanLine(y) : nullptr;

        for (int x = 0; x < w; ++x) {
            const QRgb px = line[x];

            // 蒙版命中且为不透明像素才视为图片内容;
            // 透明 PNG 的 bbox 内 alpha=0 像素实际是页面背景,仍走反色域
            if (hasImageArea && maskLine[x] == 0xFF && qAlpha(px) > 0) {
                line[x] = (ImageOriginal == opt.imagePolicy) ? px : dimPixelImage(px, opt.imageDimFactor);
            } else {
                line[x] = useLut ? invertPixelLut(px) : invertPixelExact(px, opt);
            }
        }
    }

    return img;
}

QImage applyPage(const QImage &src, const QVector<QRectF> &imageRects,
                 const Options &opt, double *coverageOut)
{
    if (coverageOut)
        *coverageOut = 0.0;
    if (src.isNull())
        return src;

    // 蒙版仅在有图片对象且策略需要分区时构建;Invert 策略 = 整页反色,无需蒙版
    if (opt.imagePolicy == ImageInvert || imageRects.isEmpty())
        return apply(src, nullptr, opt);

    const qint64 total = static_cast<qint64>(src.width()) * src.height();
    if (total <= 0)
        return apply(src, nullptr, opt);

    // bbox 面积估算覆盖率(重叠对象会略高估,阈值场景宽松无妨)
    double area = 0;
    for (const QRectF &r : imageRects)
        area += qMax(0.0, r.width()) * qMax(0.0, r.height());
    const double coverage = qMin(1.0, area / double(total));
    if (coverageOut)
        *coverageOut = coverage;

    // 扫描页特判:图片几乎铺满整页(扫描件/漫画),保留图片则白底刺眼,
    // 文字可读性优先,回退整页反色
    if (coverage > kScannedCoverThreshold)
        return apply(src, nullptr, opt);

    QImage mask(src.size(), QImage::Format_Grayscale8);
    if (mask.isNull())
        return apply(src, nullptr, opt);
    mask.fill(0);
    {
        QPainter p(&mask);
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::white);
        for (const QRectF &r : imageRects) {
            // 膨胀 2px 吸收图像边缘的抗锯齿过渡带,避免图片边缘出现反色色边
            p.drawRect(r.adjusted(-2, -2, 2, 2));
        }
    }

    return apply(src, &mask, opt);
}

} // namespace NightFilter
