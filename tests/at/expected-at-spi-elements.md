# Expected AT-SPI Elements — deepin-reader

## 推导规则

根据 `reader/app/accessible.h` 中的 `accessibleFactory` 实现，AT-SPI 名称推导规则：

1. **SET_FORM_ACCESSIBLE** 注册的类：名称 = 类名字符串（如 `"MainWindow"`, `"Central"`）
2. **Qt 通用控件 QFrame/QWidget**：名称 = `objectName()`（若空则 fallback 为 `"frame"`/`"widget"`）
3. **QPushButton**：名称 = `text()`（若空则 fallback 为 `"qpushbutton"`）
4. **DTK 控件**：名称 = `objectName()`（若空则 fallback 为类型名）
5. **显式 setAccessibleName()**：名称 = 设定的字符串（如 `"Menu_Browser"`, `"Menu_BookMark"`）
6. **DTK DSwitchButton/DFloatingButton**：优先使用 text/toolTip

## 预期元素清单

### 主窗口层（SET_FORM_ACCESSIBLE 注册）

| 预期名称 | Role | 推导链 | 使能条件 |
|----------|------|--------|---------|
| MainWindow | frame | SET_FORM_ACCESSIBLE | 应用启动即存在 |
| Central | panel | SET_FORM_ACCESSIBLE | 应用启动即存在 |
| CentralDocPage | panel | SET_FORM_ACCESSIBLE | 文档打开后存在 |
| CentralNavPage | panel | SET_FORM_ACCESSIBLE | 无文档时存在 |
| DocSheet | panel | SET_FORM_ACCESSIBLE | 文档打开后存在 |
| DocTabBar | panel | SET_FORM_ACCESSIBLE | 文档打开后存在 |
| TitleWidget | panel | SET_FORM_ACCESSIBLE | 应用启动即存在 |
| SheetSidebar | panel | SET_FORM_ACCESSIBLE | 左侧栏展开时存在 |
| SheetBrowser | panel | SET_FORM_ACCESSIBLE | 文档打开后存在 |

### 侧边栏（SET_FORM_ACCESSIBLE 注册）

| 预期名称 | Role | 推导链 | 使能条件 |
|----------|------|--------|---------|
| ThumbnailWidget | panel | SET_FORM_ACCESSIBLE | 侧边栏切换到缩略图时 |
| CatalogWidget | panel | SET_FORM_ACCESSIBLE | 侧边栏切换到目录时 |
| BookMarkWidget | panel | SET_FORM_ACCESSIBLE | 侧边栏切换到书签时 |
| NotesWidget | panel | SET_FORM_ACCESSIBLE | 侧边栏切换到注释时 |
| SearchResWidget | panel | SET_FORM_ACCESSIBLE | 侧边栏切换到搜索 |

### 侧边栏列表（setAccessibleName）

| 预期名称 | Role | 推导链 | 使能条件 |
|----------|------|--------|---------|
| Menu_BookMark | menu | setAccessibleName("Menu_BookMark") | 书签右键菜单 |
| Menu_Note | menu | setAccessibleName("Menu_Note") | 注释右键菜单 |

### 文档区域

| 预期名称 | Role | 推导链 | 使能条件 |
|----------|------|--------|---------|
| Menu_Browser | menu | setAccessibleName("Menu_Browser") | 文档区右键菜单 |
| BrowserPage | panel | SET_FORM_ACCESSIBLE | 文档打开后 |
| BrowserMagniFier | panel | SET_FORM_ACCESSIBLE | 放大镜启用时 |

### 底部工具栏

| 预期名称 | Role | 推导链 | 使能条件 |
|----------|------|--------|---------|
| PagingWidget | panel | 运行时 dump 获取 | 文档打开后 |
| Edit_Page | panel | objectName fallback | 文档打开后 |
| Edit_Page_P | panel | objectName fallback | 文档打开后 |
| thumbnailNextBtn | panel | objectName fallback | 文档打开后 |
| thumbnailPreBtn | panel | objectName fallback | 文档打开后 |

### 对话框/页面

| 预期名称 | Role | 推导链 | 使能条件 |
|----------|------|--------|---------|
| EncryptionPage | panel | 运行时 dump 获取 | 加密PDF打开时 |
| ensureBtn | panel | objectName fallback | 加密页面可见时 |
| passwdEdit | panel | objectName fallback | 加密页面可见时 |

### 菜单

| 预期名称 | Role | 推导链 | 使能条件 |
|----------|------|--------|---------|
| HandleMenu | panel | 运行时 dump 获取 | 工具选择菜单打开时 |
| defaultshape | panel | objectName fallback | HandleMenu 可见时 |
| handleshape | panel | objectName fallback | HandleMenu 可见时 |

### 注释编辑器

| 预期名称 | Role | 推导链 | 使能条件 |
|----------|------|--------|---------|
| TextEditShadowWidget | panel | objectName fallback | 注释编辑时 |
| TransparentTextEdit | panel | objectName fallback | 注释编辑时 |

### DTK 控件（通用）

| 预期名称模式 | Role | 推导链 | 说明 |
|-------------|------|--------|------|
| 动态 objectName | button/tool button | objectName | 各按钮的 objectName 值 |
| 动态 text | push button | QPushButton.text() | 按钮显示文本 |
| 动态 toolTip | floating button | DFloatingButton.toolTip() | 浮动按钮提示 |

## 运行时 vs 源码对照说明

- **运行时元素数（dump 捕获）**：17 个节点（全部 panel/container）
- **源码注册元素数（accessible.h）**：14 个 class（SET_FORM_ACCESSIBLE）
- **显式 setAccessibleName**：3 处（Menu_Browser, Menu_BookMark, Menu_Note）

**偏差分析**：
- 运行时仅捕获了底层容器节点，缺少 `MainWindow`/`Central` 等顶层 frame
- 侧边栏子页面（ThumbnailWidget/CatalogWidget/BookMarkWidget 等）需切换到对应标签页后才出现
- 文档区域元素（SheetBrowser/BrowserPage 等）需先打开文档
- 大部分按钮/输入框无 accessible_id 或 accessible_name，只能通过 objectName 作为 fallback 名称

## 关键缺口

| 缺少的元素 | 影响 | 优先级 |
|-----------|------|--------|
| 主菜单按钮（TitleMenu） | 无法定位菜单操作 | P0 |
| 查找搜索框（FindWidget） | 无法自动化查找功能 | P0 |
| 缩放控件（ScaleWidget/ScaleMenu） | 无法自动化缩放操作 | P1 |
| 幻灯片控件（SlidePlayWidget） | 无法自动化幻灯片操作 | P1 |
| 工具栏按钮 | 无法定位具体工具栏操作 | P1 |
| 子窗口/对话框（关于/打印） | 无法验证弹窗内容 | P1 |