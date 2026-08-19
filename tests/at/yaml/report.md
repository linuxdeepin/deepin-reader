# AT-SPI Test Suite Coverage Report

**生成时间**: 2026-08-19 14:41:01
**测试目录**: tests/at/yaml

---

## 1. 用例统计

| 指标 | 值 |
|------|-----|
| 总用例数 | 18 |
| 含断言用例 | 13 |
| 无断言用例 | 5 |
| 断言覆盖率 | 72.2% |

### 各模块用例分布

| 模块 | 用例数 |
|------|--------|
| annotations | 3 |
| bookmarks | 1 |
| community | 1 |
| context_menu | 1 |
| doc_open | 2 |
| doc_ops | 1 |
| keyboard | 2 |
| main_menu | 2 |
| other_interact | 1 |
| tabs | 1 |
| thumbnails | 1 |
| toc | 1 |
| window_mgmt | 1 |

## 2. AT-SPI 元素覆盖率

### 口径 A — 基于运行时元素表（elements.yaml）

| 指标 | 值 |
|------|-----|
| 已覆盖元素数 | 7 |
| 可用元素数 | 7 |
| 覆盖率 A | 100.0% |

*仅基于运行时元素表（口径 A），无源码全集对照（口径 B）。*

## 3. 重复用例检测

发现 **1** 组重复用例（相同操作序列）：

- **2 个重复**: suite_thumbnails_s1, suite_toc_s1
  - 名称: 缩略图显示, 目录导航
  - 操作序列: `mouse_click → mouse_click`

## 4. 不可自动化用例

| # | 用例 ID | 标题 | 原因 |
|---|---------|------|------|
| 1 | suite_touchscreen | 触摸屏交互 | 需要触摸屏硬件，AT-SPI无法模拟触摸手势 |
| 2 | suite_trackpad | 触摸板交互 | 需要触摸板硬件，AT-SPI无法模拟触摸板手势 |
| 3 | suite_stress | 压力测试 | 压力测试需要大量资源，不适合自动化AT测试 |
| 4 | suite_performance | 性能测试 | 性能测试依赖具体硬件平台，无法在AT测试中验证 |
| 5 | suite_compatibility | 兼容性测试 | 兼容性测试需要切换系统语言，无法在AT测试中自动化 |

---

*报告由 coverage.py 自动生成*