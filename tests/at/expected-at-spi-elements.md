# deepin-reader 预期 AT-SPI 元素清单

## 推导说明

- 命名推导基于 `getAccessibleName()` 规则（`reader/app/accessibledefine.h:22-90`）
- 前缀: `Form_` / `Button_` / `Label_` / `Editable_` / `Slider_` + `_` + accessibleName/fallback
- 同名控件自动加 `_N` 后缀
- 瞬态元素（菜单、弹窗）标注 `[TRANSIENT]`
- 静态不可达元素标注 `[INFERENCE]`

---

## 主窗口 (MainWindow)

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Form_DocMainWindow` | frame | 主窗口 | MainWindow::setAccessibleName("DocMainWindow") → Form + DocMainWindow | `MainWindow.cpp:620` |
| `Form_Central_0` | frame | 中央容器 | Central (class-based) → getAccessibleName fallback | Central.h |
| `Form_TitleWidget` | frame | 标题栏 | TitleWidget (class-based) | TitleWidget.h |
| `Form_CentralDocPage_0` | frame | 文档页堆栈 | CentralDocPage (class-based) | CentralDocPage.h |
| `Form_CentralNavPage_0` | frame | 导航页 | CentralNavPage (class-based) | CentralNavPage.h |
| `Form_ThumbnailWidget_0` | frame | 缩略图 | ThumbnailWidget (class-based) | ThumbnailWidget.h |

### 标题栏 — TitleWidget

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Label_ReaderTitle` | label | 窗口标题 | setAccessibleName("ReaderTitle") | `TitleWidget.cpp:22` |
| `Button_ReaderMenu` | push button | 主菜单按钮 | setAccessibleName("ReaderMenu") | `TitleWidget.cpp:22` → TitleMenu |
| `Label_Search Btn` | label | 主菜单按钮标题 | setObjectName("Search Btn") → default accessible | `TitleWidget.cpp:21` |

### 主菜单 — TitleMenu `[TRANSIENT]`

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Button_ReaderMenu` | push button | 菜单入口 | setAccessibleName("ReaderMenu") | `TitleMenu.cpp:43` |
| 在文件管理器中显示 | menu item | 菜单项 | objectName: `QMenu` action text | TitleMenu.cpp |
| 放大镜 | menu item | 菜单项 | action text | TitleMenu.cpp |
| 工具 | menu item | 子菜单 | action text | TitleMenu.cpp |
| 主题 | menu item | 子菜单 | action text | TitleMenu.cpp |
| 帮助 | menu item | 菜单项 | action text | TitleMenu.cpp |
| 关于 | menu item | 菜单项 | action text | TitleMenu.cpp |
| 退出 | menu item | 菜单项 | action text | TitleMenu.cpp |

### 导航页 — CentralNavPage

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Label_AddFileBtn` | label | 添加文件按钮 | setAccessibleName("AddFileBtn") | `CentralNavPage.cpp:23` |
| `Label_OpenFileBtn` | label | 打开文件按钮 | setAccessibleName("OpenFileBtn") | `CentralNavPage.cpp:37` |
| `Label_RecentView` | label | 最近文件视图 | setAccessibleName("RecentView") | `CentralNavPage.cpp:44` |
| `Label_FileItem` | label | 最近文件项 | setAccessibleName("FileItem") | `CentralNavPage.cpp:58` |

### 标签页栏 — DocTabBar

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Form_DocTabBar_0` | frame | 标签栏 | DocTabBar (class-based, QWidget fallback) | DocTabBar.h |

### 文档浏览器 — SheetBrowser

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Form_SheetBrowser_0` | frame | 文档浏览区域 | SheetBrowser (class-based) | SheetBrowser.h |
| `Label_BrowserPage_0` | label | 页面内容 | setAccessibleName("BrowserPage") | `SheetBrowser.cpp:90` |
| (空名) | layered pane | 滚动区域 | objectName 未设置 | `SheetBrowser.cpp:98` |
| `Button_PrevPage_0` | push button | 翻页按钮 | setAccessibleName("PrevPage") | `SheetBrowser.cpp:101` |

### 右键菜单 — BrowserMenu `[TRANSIENT]`

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Button_BrowserMenu` | push button | 右键菜单 | setAccessibleName("BrowserMenu") | `BrowserMenu.cpp:19` |
| 复制 | menu item | 菜单项 | action text | BrowserMenu.cpp |
| 选择全部 | menu item | 菜单项 | action text | BrowserMenu.cpp |
| 添加书签 | menu item | 菜单项 | action text | BrowserMenu.cpp |
| 放大 | menu item | 菜单项 | action text | BrowserMenu.cpp |
| 缩小 | menu item | 菜单项 | action text | BrowserMenu.cpp |

---

## 左侧栏 (SheetSidebar)

### 侧栏按钮

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Button_LeftBtn_0` | push button | 缩略图按钮 | setAccessibleName("LeftBtn") + _N | `SheetSidebar.cpp:363` |
| `Button_LeftBtn_1` | push button | 目录按钮 | setAccessibleName("LeftBtn") + _N | `SheetSidebar.cpp:363` |
| `Button_LeftBtn_2` | push button | 书签按钮 | setAccessibleName("LeftBtn") + _N | `SheetSidebar.cpp:363` |
| `Button_LeftBtn_3` | push button | 注释按钮 | setAccessibleName("LeftBtn") + _N | `SheetSidebar.cpp:363` |
| `Button_LeftBtn_4` | push button | 搜索按钮 | setAccessibleName("LeftBtn") + _N | `SheetSidebar.cpp:363` |

### 缩略图视图 — ThumbnailWidget

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Label_ThumbnailView` | label | 缩略图列表 | setAccessibleName("ThumbnailView") | `ThumbnailWidget.cpp:38` |
| `Label_SidebarListView` | label | 列表视图 | setAccessibleName("SidebarListView") | `ThumbnailWidget.cpp:43` |
| `Label_PageLabel` | label | 页码标签 | setAccessibleName("PageLabel") | `ThumbnailWidget.cpp:50` |

### 目录视图 — CatalogWidget

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Label_CatalogTree` | label | 目录树 | setAccessibleName("CatalogTree") | `CatalogWidget.cpp:40` |
| `Label_SidebarListView` | label | 列表视图 | setAccessibleName("SidebarListView") | `CatalogWidget.cpp:55` |

### 书签视图 — BookMarkWidget

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Label_BookMarkTree` | label | 书签列表 | setAccessibleName("BookMarkTree") | `BookMarkWidget.cpp:45` |
| `Label_BookMarkEdit` | label | 书签编辑 | setAccessibleName("BookMarkEdit") | `BookMarkWidget.cpp:52` |
| `Label_NoBookMark` | label | 无书签提示 | setAccessibleName("NoBookMark") | `BookMarkWidget.cpp:68` |

### 注释视图 — NotesWidget

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Label_NotesView` | label | 注释列表 | setAccessibleName("NotesView") | `NotesWidget.cpp:46` |
| `Label_NotesEdit_0`[INFERENCE] | label | 注释编辑 | setAccessibleName("NotesEdit") | `NotesWidget.cpp:55` |
| `Label_NoNotes` | label | 无注释提示 | setAccessibleName("NoNotes") | `NotesWidget.cpp:66` |

### 搜索结果视图 — SearchResWidget

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Label_SearchResult` | label | 搜索结果列表 | setAccessibleName("SearchResult") | `SearchResWidget.cpp:43` |

---

## 通用控件 (Widgets)

### 查找面板 — FindWidget

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Editable_FindInput` | entry | 查找输入框 | setAccessibleName("FindInput") | `FindWidget.cpp:120` |
| `Editable_FindText`[INFERENCE] | entry | 查找内容 | setObjectName("FindText") | `FindWidget.cpp:119,121` |
| 查找上一个 | push button | 按钮 | objectName + text | `FindWidget.cpp:129` |
| 查找下一个 | push button | 按钮 | objectName + text | `FindWidget.cpp:136` |
| 关闭 | push button | 按钮 | objectName + text | `FindWidget.cpp:143` |

### 页码控件 — PagingWidget

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Label_PageCount` | label | 页码状态 | setAccessibleName("PageCount") | `PagingWidget.cpp:74` |
| `Label_CurrentPage_0`[INFERENCE] | label | 当前页 | setAccessibleName("CurrentPage") | `PagingWidget.cpp:81` |
| `Slider_ReaderSlider` | slider | 页面滑块 | setAccessibleName("ReaderSlider") | `PagingWidget.cpp:84` |
| `Button_ReaderSliderZoomIn` | push button | 放大按钮 | setAccessibleName("ReaderSliderZoomIn") | `PagingWidget.cpp:100` |
| `Button_ReaderSliderZoomOut` | push button | 缩小按钮 | setAccessibleName("ReaderSliderZoomOut") | `PagingWidget.cpp:107` |
| `Label_SearchButton` | label | 搜索按钮 | setAccessibleName("SearchButton") | `PagingWidget.cpp:112` |

### 缩放控件 — ScaleWidget

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Button_ZoomIn`[INFERENCE] | push button | 放大 | objectName("ZoomIn") | `ScaleWidget.cpp:42` |
| (空名) | push button | 缩放比例 | objectName("ScaleComboBox") | `ScaleWidget.cpp:43` |
| `Button_ZoomOut`[INFERENCE] | push button | 缩小 | objectName("ZoomOut") | `ScaleWidget.cpp:52` |
| `Button_ScaleMenu`[INFERENCE] | push button | 缩放菜单 | objectName("ScaleMenu") | `ScaleWidget.cpp:72` |

### 幻灯片控制 — SlidePlayWidget

| 预期 AT-SPI 名称 | Role | 类型 | 推导链 | 源码位置 |
|-------------------|------|------|--------|----------|
| `Button_SlidePre_0` | push button | 上一页 | setAccessibleName("SlidePre") | `SlidePlayWidget.cpp:160` |
| `Button_SlideNext_0` | push button | 下一页 | setAccessibleName("SlideNext") | `SlidePlayWidget.cpp:160` |
| `Button_SlidePlay_0` | push button | 播放/暂停 | setAccessibleName("SlidePlay") | `SlidePlayWidget.cpp:160` |
| `Button_SlideClose_0` | push button | 关闭 | setAccessibleName("SlideClose") | `SlidePlayWidget.cpp:160` |

---

## DTK 基础控件注册表

通过 `accessibleFactory` 统一注册的 DTK/Qt 基础控件类型（通过 `USE_ACCESSIBLE` 宏创建 AT-SPI 接口）：

| 类 | Role | 说明 |
|----|------|------|
| QFrame | frame | Qt 框架容器 |
| QWidget | panel | 通用容器 |
| QPushButton | push button | Qt 按钮 |
| QSlider | slider | Qt 滑块 |
| DFrame | frame | DTK 框架容器 |
| DWidget | panel | DTK 通用容器 |
| DBackgroundGroup | panel | DTK 背景分组 |
| DSwitchButton | toggle button | DTK 开关按钮 |
| DFloatingButton | push button | DTK 浮动按钮 |
| DSearchEdit | entry | DTK 搜索框 |
| DPushButton | push button | DTK 按钮 |
| DIconButton | push button | DTK 图标按钮 |
| DCheckBox | check box | DTK 复选框 |
| DCommandLinkButton | push button | DTK 命令链接按钮 |
| DTitlebar | panel | DTK 标题栏 |
| DDialog | dialog | DTK 对话框 |
| DFileDialog | dialog | DTK 文件对话框 |

---

## 预期 vs 运行时对照说明

此预期元素清单基于源码静态推导得出。运行时 AT-SPI 树可能因以下因素差异：
- 瞬态元素（菜单、对话框）仅在触发后出现在树中
- 同名控件的 `_N` 编号取决于创建顺序
- QGraphicsItem 子类（BrowserPage, BrowserAnnotation, BrowserWord）无独立的 AT-SPI 接口
- QLabel 等静态文本控件若未设置 accessibleName，fallback 为翻译文本（不可靠的 AT 锚点）