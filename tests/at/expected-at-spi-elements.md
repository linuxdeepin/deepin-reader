# Expected AT-SPI Elements — Deepin Reader

> Generated via static code analysis (remote-codebase MCP)
> Derivation chain: accessibleFactory registration → SET_*_ACCESSIBLE macro → setAccessibleName → setObjectName fallback → getAccessibleName algorithm

---

## Naming Convention

From `accessibledefine.h::getAccessibleName()` (lines 22-90):
- **Prefix mapping**: Role → prefix
  - `QAccessible::Form` → `Form_`
  - `QAccessible::Button` → `Button_`
  - `QAccessible::StaticText` → `Label_`
  - `QAccessible::EditableText` → `Editable_`
  - `QAccessible::Slider` → `Slider_`
- **Naming priority**: `accessibleName` (setAccessibleName) > `fallback` (SET_* macro parameter) > objectName
- **Uniqueness**: Duplicates get `_N` suffix via `SEPARATOR` (underscore)

---

## Expected Elements

### 1. 主窗口 / 标题栏

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 1 | Form_DMainWindow | form | accessible.h:49 | SET_FORM_ACCESSIBLE(MainWindow, "MainWindow") | reader/MainWindow.cpp | 容器 |
| 2 | Menu_Title | menu | setAccessibleName() 显式 | Menu_Title = TitleMenu实例的accessibleName | reader/MainWindow.cpp:620 | 标题栏下拉菜单 |
| 3 | Menu_Hand | menu | setAccessibleName() 显式 | 标题栏菜单→工具子菜单 | reader/uiframe/TitleMenu.cpp:43 | 选择文本/手型工具 |
| 4 | Form_TitleWidget | form | accessible.h:55 | SET_FORM_ACCESSIBLE(TitleWidget, "TitleWidget") | reader/uiframe/TitleWidget.cpp | 标题栏工具栏容器 |
| 5 | Button_ThumbnailToggle | push button | setAccessibleName() 显式 | 缩略图面板开关按钮 | reader/uiframe/TitleWidget.cpp:22 | 初始disibled |
| 6 | Button_ThumbnailToggle_N | push button | 同名编号 | 多个同名时 _N 消歧 | — | [INFERENCE] 需运行时确认 |
| 7 | DTitlebar | form | accessible.h:82 | SET_FORM_ACCESSIBLE(DTitlebar, fallback) | — | DTK 全局标题栏 |
| 8 | DTitlebarDWindowOptionButton | push button | DTK内部 | 标题栏选项按钮 | — | 触发主菜单 |
| 9 | DTitlebarDWindowMinButton | push button | DTK内部 | 最小化按钮 | — | — |
| 10 | DTitlebarDWindowMaxButton | push button | DTK内部 | 最大化按钮 | — | — |
| 11 | DTitlebarDWindowCloseButton | push button | DTK内部 | 关闭按钮 | — | — |
| 12 | DTitlebarDWindowQuitFullscreenButton | push button | DTK内部 | 退出全屏按钮 | — | — |

### 2. 缩放控件 (ScaleWidget)

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 13 | Form_scaleEdit_P (或 scaleEdit) | form | objectName fallback | DLineEdit无accessibleName，fallback至objectName "scaleEdit_P" | reader/widgets/ScaleWidget.cpp:42 | **缺口** 缺显式setAccessibleName |
| 14 | pageEdit | editable text | objectName fallback | lineEdit对象名"scaleEdit" | reader/widgets/ScaleWidget.cpp:43 | 缩放百分比输入框 |
| 15 | Button_SP_DecreaseElement (或 DIconButton) | push button | objectName fallback | 缩小按钮 | reader/widgets/ScaleWidget.cpp:72 | **缺口** 需显式名 |
| 16 | Button_SP_IncreaseElement (或 DIconButton) | push button | objectName fallback (或 _N) | 放大按钮 | reader/widgets/ScaleWidget.cpp:79 | **缺口** 需显式名 |
| 17 | Button_editArrowBtn (或 DIconButton) | push button | objectName fallback | 缩放下拉箭头 | reader/widgets/ScaleWidget.cpp:52 | **缺口** 需显式名 |

### 3. 导航页 (CentralNavPage - 无文档时)

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 18 | Form_CentralNavPage | form | accessible.h:51 | SET_FORM_ACCESSIBLE(CentralNavPage, "CentralNavPage") | reader/uiframe/CentralNavPage.cpp | — |
| 19 | Label_Icon | label | setAccessibleName() 显式 | 文档图标 | reader/uiframe/CentralNavPage.cpp:58 | — |
| 20 | Label_Drag documents here | label | setAccessibleName() 显式 | 拖拽提示文字 | reader/uiframe/CentralNavPage.cpp:23 | 翻译文本作名 |
| 21 | Label_format supported: PDF, DJVU, DOCX | label | setAccessibleName() 显式 | 支持格式文字 | reader/uiframe/CentralNavPage.cpp:37 | 含翻译文本 |
| 22 | SelectFile | push button | setAccessibleName() 显式 | "选择文件"按钮 | reader/uiframe/CentralNavPage.cpp:44 | — |

### 4. 标签页栏 (DocTabBar)

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 23 | Form_DocTabBar | form | accessible.h:53 | SET_FORM_ACCESSIBLE(DocTabBar, "DocTabBar") | reader/uiframe/DocTabBar.cpp | 文档标签页容器 |

### 5. 文档区域 (SheetBrowser)

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 24 | Form_SheetBrowser | form | accessible.h:57 | SET_FORM_ACCESSIBLE(SheetBrowser, "SheetBrowser") | reader/browser/SheetBrowser.cpp | 文档浏览容器 |
| 25 | Tips | label | setAccessibleName() 显式 | 提示浮动控件 | reader/browser/SheetBrowser.cpp:90 | — |
| 26 | verticalScrollBar | scroll bar | setAccessibleName() 显式 | 垂直滚动条 | reader/browser/SheetBrowser.cpp:98 | — |
| 27 | horizontalScrollBar | scroll bar | setAccessibleName() 显式 | 水平滚动条 | reader/browser/SheetBrowser.cpp:101 | — |
| 28 | Menu_Browser | menu | setAccessibleName() 显式 | 文档区域右键菜单 | reader/browser/BrowserMenu.cpp:19 | — |

### 6. 侧栏 (SheetSidebar)

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 29 | Form_SheetSidebar | form | accessible.h:56 | SET_FORM_ACCESSIBLE(SheetSidebar, "SheetSidebar") | reader/sidebar/SheetSidebar.cpp | 侧栏容器 |
| 30 | Button_Catalog | push button | createBtn→setAccessibleName("Button_Catalog") | 目录按钮 | reader/sidebar/SheetSidebar.cpp:363 | — |
| 31 | Button_BookMark | push button | createBtn→setAccessibleName("Button_BookMark") | 书签按钮 | reader/sidebar/SheetSidebar.cpp:363 | — |
| 32 | Button_Note | push button | createBtn→setAccessibleName("Button_Note") | 注释按钮 | reader/sidebar/SheetSidebar.cpp:363 | — |
| 33 | Button_Thumbnail | push button | createBtn→setAccessibleName("Button_Thumbnail") | 缩略图按钮 | reader/sidebar/SheetSidebar.cpp:363 | — |
| 34 | Button_search | push button | createBtn→setAccessibleName("Button_search") | 搜索按钮 | reader/sidebar/SheetSidebar.cpp:363 | — |

### 7. 目录视图 (CatalogWidget)

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 35 | Form_CatalogWidget | form | accessible.h:59 | SET_FORM_ACCESSIBLE(CatalogWidget, "CatalogWidget") | reader/sidebar/CatalogWidget.cpp | — |
| 36 | Label_title | label | setAccessibleName() 显式 | 目录标题 | reader/sidebar/CatalogWidget.cpp:40 | — |
| 37 | View_CatalogTree | tree | setAccessibleName() 显式 | 目录树视图 | reader/sidebar/CatalogWidget.cpp:55 | — |

### 8. 书签视图 (BookMarkWidget)

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 38 | Form_BookMarkWidget | form | accessible.h:60 | SET_FORM_ACCESSIBLE(BookMarkWidget, "BookMarkWidget") | reader/sidebar/BookMarkWidget.cpp | — |
| 39 | View_ImageList | list | setAccessibleName() 显式 | 书签列表视图 | reader/sidebar/BookMarkWidget.cpp:45 | 侧栏共用名 |
| 40 | BookmarkAdd | push button | setAccessibleName() 显式 | 添加书签按钮 | reader/sidebar/BookMarkWidget.cpp:52 | — |
| 41 | BookMarkLine | divider | setAccessibleName() 显式 | 分隔线 | reader/sidebar/BookMarkWidget.cpp:68 | — |
| 42 | Menu_BookMark | menu | setAccessibleName() 显式 | 书签右键菜单 | reader/sidebar/SideBarImageListview.cpp:304 | — |

### 9. 注释视图 (NotesWidget)

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 43 | Form_NotesWidget | form | accessible.h:61 | SET_FORM_ACCESSIBLE(NotesWidget, "NotesWidget") | reader/sidebar/NotesWidget.cpp | — |
| 44 | View_ImageList | list | setAccessibleName() 显式 | 注释列表视图 | reader/sidebar/NotesWidget.cpp:46 | 侧栏共用名 |
| 45 | NotesAdd | push button | setAccessibleName() 显式 | 添加注释按钮 | reader/sidebar/NotesWidget.cpp:55 | — |
| 46 | NotesLine | divider | setAccessibleName() 显式 | 分隔线 | reader/sidebar/NotesWidget.cpp:66 | — |
| 47 | Menu_Note | menu | setAccessibleName() 显式 | 注释右键菜单 | reader/sidebar/SideBarImageListview.cpp:276 | — |

### 10. 搜索结果视图 (SearchResWidget)

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 48 | Form_SearchResWidget | form | accessible.h:62 | SET_FORM_ACCESSIBLE(SearchResWidget, "SearchResWidget") | reader/sidebar/SearchResWidget.cpp | — |
| 49 | View_ImageList | list | setAccessibleName() 显式 | 搜索结果列表 | reader/sidebar/SearchResWidget.cpp:43 | 侧栏共用名 |

### 11. 缩略图视图 (ThumbnailWidget)

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 50 | Form_ThumbnailWidget | form | accessible.h:58 | SET_FORM_ACCESSIBLE(ThumbnailWidget, "ThumbnailWidget") | reader/sidebar/ThumbnailWidget.cpp | — |
| 51 | View_ImageList | list | setAccessibleName() 显式 | 缩略图列表 | reader/sidebar/ThumbnailWidget.cpp:38 | 侧栏共用名 |
| 52 | ThumbnailLine | divider | setAccessibleName() 显式 | 分隔线 | reader/sidebar/ThumbnailWidget.cpp:50 | — |

### 12. 翻页控件 (PagingWidget)

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 53 | Paging | form | setAccessibleName() 显式 | 翻页控件自身 | reader/sidebar/ThumbnailWidget.cpp:43 | — |
| 54 | Label_TotalPage | label | setAccessibleName() 显式 | 总页数标签 | reader/widgets/PagingWidget.cpp:74 | — |
| 55 | Page | editable text | setAccessibleName() 显式 | 跳转页输入框(DLineEdit) | reader/widgets/PagingWidget.cpp:81 | — |
| 56 | pageEdit | editable text | setAccessibleName() 显式 | 跳转页输入框(lineEdit) | reader/widgets/PagingWidget.cpp:84 | — |
| 57 | Button_ThumbnailPre | push button | setAccessibleName() 显式 | 上一页按钮 | reader/widgets/PagingWidget.cpp:100 | — |
| 58 | Button_ThumbnailNext | push button | setAccessibleName() 显式 | 下一页按钮 | reader/widgets/PagingWidget.cpp:107 | — |
| 59 | CurrentPage | label | setAccessibleName() 显式 | 当前页标签 | reader/widgets/PagingWidget.cpp:112 | 初始不可见 |

### 13. 查找控件 (FindWidget)

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 60 | Form_findSearchEdit_P | form | setAccessibleName() 显式 | 查找输入框(DSearchEdit) | reader/widgets/FindWidget.cpp:120 | — |
| 61 | DLineEditChildLineEdit | editable text | setAccessibleName() 显式 | 查找输入框(lineEdit) | reader/widgets/FindWidget.cpp:122 | — |
| 62 | Button_SP_ArrowUpBtn (或 DIconButton) | push button | objectName fallback | 查找上一个 | reader/widgets/FindWidget.cpp:129 | **缺口** |
| 63 | Button_SP_ArrowDownBtn (或 DIconButton) | push button | objectName fallback | 查找下一个 | reader/widgets/FindWidget.cpp:136 | **缺口** |
| 64 | Button_closeButton (或 DDialogCloseButton) | push button | objectName fallback | 关闭查找按钮 | reader/widgets/FindWidget.cpp:143 | **缺口** |

### 14. 其他控件

| # | AT-SPI 名称 | Role | 源码来源 | 推导链 | 文件:行号 | 备注 |
|---|-------------|------|----------|--------|-----------|------|
| 65 | Form_EncryptionPage (或 Form_widget) | form | accessible.h fallback | 加密页面，无setAccessibleName | reader/widgets/EncryptionPage.cpp | **缺口** |
| 66 | passwdEdit | editable text | objectName fallback | 密码输入框 | reader/widgets/EncryptionPage.cpp:46 | **缺口** |
| 67 | ensureBtn | push button | objectName fallback | 确认按钮 | reader/widgets/EncryptionPage.cpp:50 | **缺口** |
| 68 | Form_RestoreTipWidget (或 Form_widget) | form | accessible.h fallback | 恢复位置提示条 | reader/widgets/RestoreTipWidget.cpp | **缺口** |
| 69 | Form_DocSheet | form | accessible.h:52 | SET_FORM_ACCESSIBLE(DocSheet, "DocSheet") | reader/uiframe/DocSheet.cpp | 文档打开后的容器 |
| 70 | Form_CentralDocPage | form | accessible.h:50 | SET_FORM_ACCESSIBLE(CentralDocPage, "CentralDocPage") | reader/uiframe/CentralDocPage.cpp | 文档页面容器 |
| 71 | Form_Central | form | accessible.h:48 | SET_FORM_ACCESSIBLE(Central, "Central") | reader/uiframe/Central.cpp | 中央容器 |

### 15. 瞬态元素（需运行时确认）

| # | AT-SPI 名称 | Role | 触发场景 | 推导链 | 备注 |
|---|-------------|------|----------|--------|------|
| 72 | ScaleMenu (DMenu) | menu | 点击缩放输入框箭头 | 无setAccessibleName，fallback至objectName | [INFERENCE] |
| 73 | SlidePlayWidget按钮们 | push button | 幻灯片播放模式 | createBtn只设objectName | [INFERENCE] |
| 74 | TextEditWidget | editable text | 注释编辑 | 无setAccessibleName | [INFERENCE] |

---

## 命名分类统计

| 分类 | 计数 | 说明 |
|------|------|------|
| ✅ 显式 setAccessibleName | 30+ | 已存在可靠AT-SPI名 |
| ✅ 类级 AT 注册 | 14 | 通过SET_FORM_ACCESSIBLE注册的app类 |
| ⚠️ objectName fallback | 10 | 仅依赖objectName，可改进 |
| 🔴 缺口 (无任何锚点) | 6 | 见 checklist |
| [INFERENCE] 需运行时确认 | 5 | 动态生成/条件可见/同名消歧 |