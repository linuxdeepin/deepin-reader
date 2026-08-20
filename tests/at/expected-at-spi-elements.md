# deepin-reader 预期 AT-SPI 元素清单

> **推导方法**: 基于源码静态分析（accessible.h 注册表 + setAccessibleName + setObjectName + 命名算法 getAccessibleName）
> **命名算法**: `{RolePrefix}_{accessibleName || objectName || fallback}`，重复加 `_N`（参见 accessibledefine.h）
> **状态**: 不含运行时验证，`[INFERENCE]` 标记需要运行时确认

---

## 1. 顶层窗口

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_MainWindow` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:47 | — |

## 2. 标题栏 (TitleWidget / DTitlebar)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_TitleWidget` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:53 | — |
| `Button_ThumbnailToggle` | Button | `setAccessibleName("Button_ThumbnailToggle")` | TitleWidget.cpp:20 | — |
| `Form_DTitlebar` | Form | SET_FORM_ACCESSIBLE 通用注册，objectName=空 → `Form_DTitlebar` | accessible.h:80 | objectName 确认 |
| `DTitlebarDWindowOptionButton` | Button | DIconButton → accessibleName→objectName | DTitlebar | 运行时确认 |
| `DTitlebarDWindowQuitFullscreenButton` | Button | DIconButton → objectName | DTitlebar | 运行时确认 |
| `DTitlebarDWindowMinButton` | Button | DIconButton → objectName | DTitlebar | 运行时确认 |
| `DTitlebarDWindowMaxButton` | Button | DIconButton → objectName | DTitlebar | 运行时确认 |
| `DTitlebarDWindowCloseButton` | Button | DIconButton → objectName | DTitlebar | 运行时确认 |

## 3. 缩放控件 (ScaleWidget)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_scaleEdit_P` | Form | DLineEdit, objectName=scaleEdit_P → `Form_scaleEdit_P` | ScaleWidget.cpp:42 | — |
| `SP_DecreaseElement` | Button | DIconButton, objectName=SP_DecreaseElement | ScaleWidget.cpp:72, TitleWidget.cpp:32 | 可能被父类引用覆盖 |
| `SP_IncreaseElement` | Button | DIconButton, objectName=SP_IncreaseElement | ScaleWidget.cpp:79, TitleWidget.cpp:35 | 同上 |
| `editArrowBtn` | Button | DIconButton, objectName=editArrowBtn | ScaleWidget.cpp:52 | — |

> 注：`SP_DecreaseElement` 和 `SP_IncreaseElement` 同时存在于 TitleWidget.cpp 的 ScaleWidget 子查找中，但 AT 名受 DIconButton 通用注册影响 → **`[INFERENCE]` 运行时确认实际 AT 名**

## 4. 标签页栏 (DocTabBar)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_DocTabBar` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:52 | — |
| `leftButton` | Button | DIconButton → objectName → `Button_leftButton` | CentralDocPage.cpp:120 | 运行时确认 |
| `rightButton` | Button | DIconButton → objectName → `Button_rightButton` | CentralDocPage.cpp:121 | 运行时确认 |
| `AddButton` | Button | DIconButton → objectName → `Button_AddButton` | CentralDocPage.cpp:122 | 运行时确认 |
| 标签1..N 文本 | Button | DTabBar 内部 tab → 通用注册 | DTabBar | DTK 内部，运行时确认 |

## 5. 中央区域

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_Central` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:48 | — |
| `Form_CentralDocPage` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:49 | — |
| `Form_CentralNavPage` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:50 | — |

## 6. 文档标签 (DocSheet)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_DocSheet` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:51 | — |

## 7. 文档浏览区 (SheetBrowser)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_SheetBrowser` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:55 | — |
| `verticalScrollBar` | Button/ScrollBar | QScrollBar → setAccessibleName | SheetBrowser.cpp:100 | — |
| `horizontalScrollBar` | Button/ScrollBar | QScrollBar → setAccessibleName | SheetBrowser.cpp:107 | — |
| `Tips` | Label/Form | TipsWidget → setAccessibleName | SheetBrowser.cpp:90 | — |

## 8. 左侧栏 (SheetSidebar)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_SheetSidebar` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:54 | — |
| `Button_Catalog` | Button | DToolButton, setAccessibleName("Button_"+objName) | SheetSidebar.cpp:363 | objName 确认 |
| `Button_BookMark` | Button | 同上 | SheetSidebar.cpp:363 | objName 确认 |
| `Button_Notes` | Button | 同上 | SheetSidebar.cpp:363 | objName 确认 |
| `Button_Search` | Button | 同上 | SheetSidebar.cpp:363 | 无 TabFocus |
| `Button_Thumbnail` | Button | 同上 | SheetSidebar.cpp:363 | objName 确认 |

### 8.1 目录视图 (CatalogWidget)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_CatalogWidget` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:57 | — |
| Catalog tree items | Tree | DTreeView 子项 | CatalogWidget.cpp | DTK 内部 |

### 8.2 书签视图 (BookMarkWidget)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_BookMarkWidget` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:58 | — |
| BookMark list items | list | DListView 子项 | BookMarkWidget.cpp | DTK 内部 |

### 8.3 注释视图 (NotesWidget)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_NotesWidget` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:59 | — |
| Notes list items | list | DListView 子项 | NotesWidget.cpp | DTK 内部 |

### 8.4 搜索结果视图 (SearchResWidget)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_SearchResWidget` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:60 | — |
| Search results list | list | DListView 子项 | SearchResWidget.cpp | DTK 内部 |

### 8.5 缩略图视图 (ThumbnailWidget)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_ThumbnailWidget` | Form | SET_FORM_ACCESSIBLE 固定名 | accessible.h:56 | — |

## 9. 搜索栏 (FindWidget)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_findSearchEdit_P` | Form | DSearchEdit, setAccessibleName | FindWidget.cpp:120 | — |
| `DLineEditChildLineEdit` | Editable | 同上行，lineEdit 子控件 | FindWidget.cpp:122 | — |
| `SP_ArrowUpBtn` | Button | DIconButton → objectName | FindWidget.cpp:129 | — |
| `SP_ArrowDownBtn` | Button | DIconButton → objectName | FindWidget.cpp:136 | — |
| `closeButton` | Button | DDialogCloseButton → objectName | FindWidget.cpp:143 | — |

## 10. 菜单

### 10.1 标题栏主菜单

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Menu_Title` | Menu | DMenu, setAccessibleName("Menu_Title") | MainWindow.cpp:620 | — |
| `New window` | MenuItem | QAction 文本(tr)，DTK DMenu 渲染 | TitleMenu.cpp:18 | 运行时确认 |
| `New tab` | MenuItem | QAction 文本 | TitleMenu.cpp:18 | 同上 |
| `Save` | MenuItem | QAction 文本 | TitleMenu.cpp:24 | 同上 |
| `Save as` | MenuItem | QAction 文本 | TitleMenu.cpp:24 | 同上 |
| `Display in file manager` | MenuItem | QAction 文本 | TitleMenu.cpp:28 | 同上 |
| `Magnifer` | MenuItem | QAction 文本 | TitleMenu.cpp:28 | 同上 |
| EyeProtection menu | Menu | EyeProtectionAction 子菜单 | TitleMenu.cpp:33 | 运行时确认 |
| `Menu_Hand` | Menu | DMenu, setAccessibleName("Menu_Hand") | TitleMenu.cpp:43 | — |
| Select Text (选择文本) | MenuItem | QAction 文本 | HandleMenu.cpp:24 | 运行时确认 |
| Hand Tool (抓手工具) | MenuItem | QAction 文本 | HandleMenu.cpp:31 | 运行时确认 |
| `Search` | MenuItem | QAction 文本 | TitleMenu.cpp:46 | 同上 |
| `Print` | MenuItem | QAction 文本 | TitleMenu.cpp:46 | 同上 |

### 10.2 缩放比例菜单 (ScaleMenu)

| 名称 | Role | AT名推导 | 源码来源 |
|------|------|---------|---------|
| 缩放菜单 DMenu | Menu | DMenu, 无显式 accessibleName → fallback | ScaleMenu |
| 200%/150%/125%/100%/75%/50% | MenuItem | QAction 文本 | ScaleMenu.cpp |
| Fit width/Fit Height | MenuItem | QAction 文本 | ScaleMenu.cpp |

### 10.3 文档右键菜单 (BrowserMenu)

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| BrowserMenu DMenu | Menu | DMenu, 无显式 accessibleName → fallback | BrowserMenu | 运行时确认 |
| 各项菜单项 | MenuItem | QAction 文本 | BrowserMenu.cpp:24-212 | 运行时确认 |

## 11. 对话框

| 名称 | Role | AT名推导 | 源码来源 | [INFERENCE] |
|------|------|---------|---------|-------------|
| `Form_DDialog` (通用) | Form | DDialog 通用注册 → objectName/备用 | accessible.h:82 | — |
| `Form_DFileDialog` | Form | DFileDialog 通用注册 | accessible.h:83 | — |
| EncryptionPage | Form | DWidget 通用注册 | EncryptionPage.cpp | 运行时确认 |
| SaveDialog | Form | DDialog, objectName | SaveDialog | 运行时确认 |
| ProgressDialog | Form | DDialog, objectName | ProgressDialog | 运行时确认 |
| FileAttrWidget | Form | DDialog, objectName | FileAttrWidget | 运行时确认 |
| ShortCutShow | Form | 独立dialog | ShortCutShow | 运行时确认 |
| SecurityDialog | Form | DDialog | SecurityDialog | 运行时确认 |

## 12. 合计

| 类别 | 静态可确认 | [INFERENCE] 需运行时 | 合计 |
|------|-----------|-------------------|------|
| 顶层窗口 | 1 | 0 | 1 |
| 标题栏 | 3 | 5 | 8 |
| 缩放控件 | 4 | 2 | 6 |
| 标签页栏 | 1 | 4 | 5 |
| 中央区域 | 3 | 0 | 3 |
| 文档标签 | 1 | 0 | 1 |
| 文档浏览区 | 4 | 0 | 4 |
| 左侧栏 | 6 | 5 | 11 |
| 搜索栏 | 5 | 0 | 5 |
| 菜单 | 3 | 32+ | 35+ |
| 对话框 | 2 | 6 | 8 |
| **总计** | **33** | **54+** | **87+** |