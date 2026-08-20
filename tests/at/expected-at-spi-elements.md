# Deepin Reader 预期 AT-SPI 元素清单

> 生成时间: 2026-08-20
> 推导方式: 源码静态分析 via remote-codebase MCP + 运行时 at-tree 数据

## 预期元素表

### 主窗口

| AT 名称 | Role | 功能 | 触发条件 | 使能条件 | 推导链 |
|---------|------|------|----------|----------|--------|
| Form_DMainWindow | form | 主窗口 | 应用启动 | 始终 | SET_FORM_ACCESSIBLE(MainWindow) |
| Form_DMainWindowTitlebar | form | 标题栏 | 主窗口创建 | 始终 | DTK fallback |
| Form_DTitlebarLeftArea | form | 标题栏左侧区域 | 主窗口创建 | 始终 | DTK fallback |
| Form_DTitlebarRightArea | form | 标题栏右侧区域 | 主窗口创建 | 始终 | DTK fallback |
| Form_DTitlebarButtonArea | form | 窗口按钮区域 | 主窗口创建 | 始终 | DTK fallback |
| DTitlebarIconLabel | button | 应用图标 | 主窗口创建 | 始终 | DTK fallback: DTitlebarIconLabel |
| DTitlebarDWindowOptionButton | button | 主菜单按钮 | 主窗口创建 | 始终 | DTK fallback |
| DTitlebarDWindowMinButton | button | 最小化 | 主窗口创建 | 始终 | DTK fallback |
| DTitlebarDWindowMaxButton | button | 最大化 | 主窗口创建 | 始终 | DTK fallback |
| DTitlebarDWindowQuitFullscreenButton | button | 退出全屏 | 主窗口创建 | 全屏状态 | DTK fallback |
| DTitlebarDWindowCloseButton | button | 关闭 | 主窗口创建 | 始终 | DTK fallback |
| Form_TitleWidget | form | 标题栏工具栏 | 主窗口创建 | 始终 | SET_FORM_ACCESSIBLE(TitleWidget) |
| Button_ThumbnailToggle | check box | 缩略图侧栏开关 | 标题栏工具栏 | 始终 | setAccessibleName("Button_ThumbnailToggle") |
| Form_scaleEdit_P | form | 缩放比例输入区 | 文档打开后 | 有文档打开 | DTK fallback |
| DLineEditChildLineEdit | text | 缩放比例输入框 | 文档打开后 | 有文档打开 | Qt fallback: DLineEditChildLineEdit |

### 中央区域

| AT 名称 | Role | 功能 | 触发条件 | 使能条件 | 推导链 |
|---------|------|------|----------|----------|--------|
| Form_Central | form | 中央区域 | 主窗口创建 | 始终 | SET_FORM_ACCESSIBLE(Central) |
| Form_CentralNavPage | form | 空状态页面 | 无文档打开 | 无文档打开 | SET_FORM_ACCESSIBLE(CentralNavPage) |
| Button_SelectFile | button | 选择文件按钮 | 空状态页 | 无文档打开 | setAccessibleName("Button_SelectFile") |
| Label_Icon | image | 空状态图标 | 空状态页 | 无文档打开 | setAccessibleName("Label_Icon") |
| Form_CentralDocPage | form | 文档页面 | 有文档打开 | 有文档打开 | SET_FORM_ACCESSIBLE(CentralDocPage) |
| Form_DocTabBar | form | 文档标签栏 | 有文档打开 | 有文档打开 | SET_FORM_ACCESSIBLE(DocTabBar) |
| 向左滚动 | button | 向左滚动标签 | 标签过多 | 标签溢出 | DTK fallback（文本） |
| 向右滚动 | button | 向右滚动标签 | 标签过多 | 标签溢出 | DTK fallback（文本） |
| DTabBarAddButton | button | 新建标签页 | 有文档打开 | 始终 | DTK fallback |
| Form_DocSheet | form | 文档分栏 | 文档打开后 | 始终 | SET_FORM_ACCESSIBLE(DocSheet) |
| Form_SheetSidebar | form | 侧栏容器 | 侧栏显示 | 缩略图开关打开 | SET_FORM_ACCESSIBLE(SheetSidebar) |
| Form_SheetBrowser | form | 文档浏览区域 | 文档打开后 | 始终 | SET_FORM_ACCESSIBLE(SheetBrowser) |
| Form_FindWidget | form | 查找框 | Ctrl+F 触发 | 有文档打开 | SET_FORM_ACCESSIBLE(FindWidget) |
| Form_BrowserPage | form | 文档页面渲染 | 文档打开后 | 始终 | SET_FORM_ACCESSIBLE(BrowserPage) |

### 子组件（无 explicit AT-SPI 名称 — 见 checklist 缺口项）

以下组件运行时会出现在 AT-SPI 树中，但名称为泛型 DTK/Qt fallback 或空名称：

| 运行时名称 | Role | 所在组件 | 推导链 | 缺口编号 |
|-----------|------|---------|--------|---------|
| （空/泛型） | panel | ScaleWidget | 无 setAccessibleName | G01 |
| （空/泛型） | push button | ScaleWidget 缩放按钮 | 无 setAccessibleName | G02 |
| （空/泛型） | panel | SlidePlayWidget | 无 setAccessibleName | G03 |
| （空/泛型） | push button | SlidePlayWidget 控制按钮 | 无 setAccessibleName | G04 |
| （空/泛型） | panel | SlideWidget | 无 setAccessibleName | G05 |
| （空/泛型） | panel | EncryptionPage | 无 setAccessibleName | G06 |
| （空/泛型） | push button | EncryptionPage 按钮 | 无 setAccessibleName | G07 |
| （空/泛型） | panel | HandleMenu | 无 setAccessibleName | G08 |
| （空/泛型） | panel | TextEditShadowWidget | 无 setAccessibleName | G09 |
| （空/泛型） | panel | RestoreTipWidget | 无 setAccessibleName | G10 |
| （空/泛型） | dialog | ProgressDialog | 无 setAccessibleName | G11 |
| （空/泛型） | push button | ProgressDialog 按钮 | 无 setAccessibleName | G12 |
| （空/泛型） | dialog | SecurityDialog | 无 setAccessibleName | G13 |
| （空/泛型） | push button | SecurityDialog 按钮 | 无 setAccessibleName | G14 |

### 主菜单

| AT 名称 | Role | 功能 | 触发条件 | 使能条件 |
|---------|------|------|----------|----------|
| DTitlebarMainMenu | popup menu | 主菜单弹出框 | 点击主菜单按钮 | 始终 |
| 新窗口 | menu item | 打开新窗口 | 主菜单 | 始终 |
| 新标签页 | menu item | 新建标签页 | 主菜单 | 始终 |
| 保存 | menu item | 保存文档 | 主菜单 | 有文档打开 |
| 另存为 | menu item | 另存文档 | 主菜单 | 有文档打开 |
| 在文件管理器中显示 | menu item | 定位文件位置 | 主菜单 | 有文档打开 |
| 放大镜 | menu item | 放大镜模式 | 主菜单 | 有文档打开 |
| 工具 | menu item | 工具子菜单 | 主菜单 | 有文档打开 |
| 设置 | menu item | 打开设置 | 主菜单 | 始终 |
| 帮助 | menu item | 帮助子菜单 | 主菜单 | 始终 |

### 右键菜单（BrowserMenu）

| AT 名称 | Role | 功能 | 触发条件 |
|---------|------|------|----------|
| Form_BrowserMenu | form | 右键菜单容器 | 右键点击文档区域 |
| 复制 | menu item | 复制选中文本 | 有选中文本 |
| 提取文本 | menu item | OCR 提取文本 | 始终 |
| 翻译 | menu item | 翻译选中文本 | 始终 |
| 截图 | menu item | 截图 | 始终 |
| 全屏 | menu item | 全屏模式 | 始终 |
| 朗读 | menu item | TTS 朗读 | 有选中文本 |
| 打印 | menu item | 打印文档 | 始终 |
| 选择文本 | menu item | 切换文本选择模式 | 始终 |
| 放大 | menu item | 放大视图 | 始终 |
| 缩小 | menu item | 缩小视图 | 始终 |
| 适应宽度 | menu item | 适应宽度显示 | 始终 |
| 适应高度 | menu item | 适应高度显示 | 始终 |
| 双页显示 | menu item | 双页模式 | 始终 |
| 滚动方向 | menu item | 滚动方向子菜单 | 始终 |
| 旋转 | menu item | 旋转子菜单 | 始终 |

### 对话框

| AT 名称 | Role | 功能 | 触发条件 |
|---------|------|------|----------|
| DDialog（设置） | dialog | 设置对话框 | 菜单 → 设置 |
| DAboutDialog（关于） | dialog | 关于对话框 | 菜单 → 帮助 → 关于 |
| DShortcutShow | dialog | 快捷键提示 | 菜单 → 帮助 → 帮助 |
| DDialog（保存） | dialog | 保存确认 | 关闭未保存文档 |
| DDialog（安全） | dialog | 安全提醒 | 打开加密文档 |
| DDialog（进度） | dialog | 进度显示 | 导出/打印操作 |
| DDialog（文件属性） | dialog | 文件属性 | 菜单 → 信息 |

## 元素统计

### 总数

| 分类 | 数量 | 说明 |
|------|------|------|
| 已注册自定义控件 | 14 | SET_FORM_ACCESSIBLE 注册类 |
| 有显式 setAccessibleName | 3 | Button_ThumbnailToggle, Button_SelectFile, Label_Icon |
| DTK/Qt fallback 自动命名 | ~25 | 标题栏按钮、菜单项、标签栏等 |
| 缺口（无 AT 名称） | 14 | 子组件无 setAccessibleName |
| 总计 | ~56 | 含容器、可交互控件、菜单项 |

### 可交互控件统计

| 可交互控件 | 已覆盖 | 缺口 | 覆盖率 |
|-----------|--------|------|--------|
| 按钮 | 12 | 6 | 67% |
| 输入框 | 1 | 0 | 100% |
| 复选框 | 1 | 0 | 100% |
| 菜单项 | 20+ | 0 | 100% |
| 面板/容器 | 7 | 6 | 54% |
| 对话框 | 5 | 2 | 71% |

> 注：菜单项使用文本标记，不属于 AT-SPI 名称缺口。缺口仅针对 setAccessibleName 缺失的子组件。