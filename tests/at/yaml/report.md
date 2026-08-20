# AT-SPI Test Suite Coverage Report

**生成时间**: 2026-08-20 14:02:47
**测试目录**: tests/at/yaml

---

## 1. 用例统计

| 指标 | 值 |
|------|-----|
| 总用例数 | 25 |
| 含断言用例 | 25 |
| 无断言用例 | 0 |
| 断言覆盖率 | 100.0% |

### 各模块用例分布

| 模块 | 用例数 |
|------|--------|
| 左侧栏—书签视图 | 2 |
| 左侧栏—注释视图 | 3 |
| 左侧栏—目录视图 | 1 |
| 左侧栏—缩略图视图 | 1 |
| 文档区域 | 2 |
| 文档区域(右键菜单) | 1 |
| 文档区域(快捷键) | 2 |
| 文档区域_标题栏 | 7 |
| 文档区域_标题栏(综合) | 1 |
| 标题栏主菜单 | 2 |
| 标题栏窗口按钮 | 1 |
| 顶部标签页栏 | 2 |

## 2. AT-SPI 元素覆盖率

### 口径 A — 基于运行时元素表（elements.yaml）

| 指标 | 值 |
|------|-----|
| 已覆盖元素数 | 0 |
| 可用元素数 | 35 |
| 覆盖率 A | 0.0% |

*仅基于运行时元素表（口径 A），无源码全集对照（口径 B）。*

### 未覆盖元素

| 名称 | 角色 | 引用键 |
|------|------|--------|
| Button_SelectFile | button | n36 |
| Button_ThumbnailToggle | check box | n8 |
| DLineEditChildLineEdit | text | n11 |
| DTabBarAddButton | button | n30 |
| DTitlebarDWindowCloseButton | button | n22 |
| DTitlebarDWindowMaxButton | button | n20 |
| DTitlebarDWindowMinButton | button | n19 |
| DTitlebarDWindowOptionButton | button | n18 |
| DTitlebarDWindowQuitFullscreenButton | button | n21 |
| DTitlebarIconLabel | button | n6 |
| 主题 | menu item | n52 |
| 保存 | menu item | n42 |
| 关于 | menu item | n58 |
| 另存为 | menu item | n43 |
| 向右滚动 | button | n29 |
| 向左滚动 | button | n28 |
| 在文件管理器中显示 | menu item | n44 |
| 工具 | menu item | n46 |
| 帮助 | menu item | n57 |
| 手形工具 | menu item | n49 |
| 手形工具 | menu item | n62 |
| 打印 | menu item | n51 |
| 搜索 | menu item | n50 |
| 放大镜 | menu item | n45 |
| 新标签页 | menu item | n41 |
| 新窗口 | menu item | n40 |
| 浅色 | menu item | n54 |
| 浅色 | menu item | n64 |
| 深色 | menu item | n55 |
| 深色 | menu item | n65 |
| ... 共 35 个未覆盖元素，仅显示前 30 | | |

## 3. 重复用例检测

未发现重复用例。

## 5. 缺口分析

共有 **35** 个 AT-SPI 元素未被任何用例引用。
可能原因：
- 元素在当前测试场景中不可达（条件渲染、窗口未打开）
- 元素缺少 `setAccessibleName()`，无法通过名称定位
- 用例未覆盖该功能路径

---

*报告由 coverage.py 自动生成*