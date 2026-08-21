# deepin-reader 预期 AT-SPI 元素清单

## 命名规则

根据 `reader/app/accessibledefine.h` 中 `getAccessibleName()` 函数：

```
Name = RolePrefix + SEPARATOR + (accessibleName || fallback)
```

其中：
- `RolePrefix` = `Form` / `Button` / `Label` / `Editable` / `Slider`（根据 QAccessible::Role）
- `SEPARATOR` = `"_"`
- 若 `accessibleName` 非空，优先使用
- 否则使用 `fallback`（即 `SET_*_ACCESSIBLE` 宏第二个参数）
- 若同名冲突，追加 `SEPARATOR + N`（递增编号）

**注册工厂**: `accessibleFactory` 在 `reader/app/accessible.h:87-130` 中注册，在 `main.cpp:108` 通过 `QAccessible::installFactory(accessibleFactory)` 安装。

---

## 1. 注册应用层控件（有类级 AT 注册 — 通过 `SET_FORM_ACCESSIBLE` / `SET_BUTTON_ACCESSIBLE`）

这些控件通过 `accessibleFactory` 从 `accessible.h` 注册，AT 名规则由 `getAccessibleName()` 决定。

| 预期 AT-SPI 名称 | 控件 | Role | 推导链 | 文件:行 |
|-----------------|------|------|--------|---------|
| `Form_MainWindow` | MainWindow | Form | SET_FORM_ACCESSIBLE("MainWindow") → Form_MainWindow | accessible.h:49 |
| `Form_Central` | Central | Form | SET_FORM_ACCESSIBLE("Central") → Form_Central | accessible.h:50 |
| `Form_CentralDocPage` | CentralDocPage | Form | SET_FORM_ACCESSIBLE("CentralDocPage") → Form_CentralDocPage | accessible.h:51 |
| `Form_CentralNavPage` | CentralNavPage | Form | SET_FORM_ACCESSIBLE("CentralNavPage") → Form_CentralNavPage | accessible.h:52 |
| `Form_DocSheet` | DocSheet | Form | SET_FORM_ACCESSIBLE("DocSheet") → Form_DocSheet | accessible.h:53 |
| `Form_DocTabBar` | DocTabBar | Form | SET_FORM_ACCESSIBLE("DocTabBar") → Form_DocTabBar | accessible.h:54 |
| `Form_TitleWidget` | TitleWidget | Form | SET_FORM_ACCESSIBLE("TitleWidget") → Form_TitleWidget | accessible.h:55 |
| `Form_SheetSidebar` | SheetSidebar | Form | SET_FORM_ACCESSIBLE("SheetSidebar") → Form_SheetSidebar | accessible.h:56 |
| `Form_SheetBrowser` | SheetBrowser | Form | SET_FORM_ACCESSIBLE("SheetBrowser") → Form_SheetBrowser | accessible.h:57 |
| `Form_ThumbnailWidget` | ThumbnailWidget | Form | SET_FORM_ACCESSIBLE("ThumbnailWidget") → Form_ThumbnailWidget | accessible.h:58 |
| `Form_CatalogWidget` | CatalogWidget | Form | SET_FORM_ACCESSIBLE("CatalogWidget") → Form_CatalogWidget | accessible.h:59 |
| `Form_BookMarkWidget` | BookMarkWidget | Form | SET_FORM_ACCESSIBLE("BookMarkWidget") → Form_BookMarkWidget | accessible.h:60 |
| `Form_NotesWidget` | NotesWidget | Form | SET_FORM_ACCESSIBLE("NotesWidget") → Form_NotesWidget | accessible.h:61 |
| `Form_SearchResWidget` | SearchResWidget | Form | SET_FORM_ACCESSIBLE("SearchResWidget") → Form_SearchResWidget | accessible.h:62 |
| `Form_<objectName>` 或 `Form_frame` | QFrame | Form | SET_FORM_ACCESSIBLE(QFrame, ...) → 取 objectName 或 "frame" | accessible.h:65 |
| `Form_<objectName>` 或 `Form_widget` | QWidget | Form | SET_FORM_ACCESSIBLE(QWidget, ...) → 取 objectName 或 "widget" | accessible.h:66 |
| `Button_<text>` 或 `Button_qpushbutton` | QPushButton | Button | SET_BUTTON_ACCESSIBLE(QPushButton, ...) → 取 text() 或 "qpushbutton" | accessible.h:67 |
| `Slider_qslider` | QSlider | Slider | SET_SLIDER_ACCESSIBLE(QSlider, "qslider") → Slider_qslider | accessible.h:68 |
| `Form_<objectName>` 或 `Form_frame` | DFrame | Form | 取 objectName 或 "frame" | accessible.h:71 |
| `Form_<objectName>` 或 `Form_widget` | DWidget | Form | 取 objectName 或 "widget" | accessible.h:72 |
| `Form_<objectName>` 或 `Form_dbackgroundgroup` | DBackgroundGroup | Form | 取 objectName 或 "dbackgroundgroup" | accessible.h:73 |
| `Button_<text>` 或 `Button_switchbutton` | DSwitchButton | Button | 取 text() 或 "switchbutton" | accessible.h:74 |
| `Button_<toolTip>` 或 `Button_DFloatingButton` | DFloatingButton | Button | 取 toolTip() 或 "DFloatingButton" | accessible.h:75 |
| `Form_<objectName>` 或 `Form_DSearchEdit` | DSearchEdit | Form | 取 objectName 或 "DSearchEdit" | accessible.h:76 |
| `Button_<objectName>` 或 `Button_DPushButton` | DPushButton | Button | 取 objectName 或 "DPushButton" | accessible.h:77 |
| `Button_<objectName>` 或 `Button_DIconButton` | DIconButton | Button | 取 objectName 或 "DIconButton" | accessible.h:78 |
| `Button_<objectName>` 或 `Button_DCheckBox` | DCheckBox | Button | 取 objectName 或 "DCheckBox" | accessible.h:79 |
| `Button_DCommandLinkButton` | DCommandLinkButton | Button | 固定名 | accessible.h:80 |
| `Label_<objectName>` 或 `Label_DLabel` | DLabel | StaticText | 取 objectName 或 "DLabel" | accessible.h:81 |
| `Form_<objectName>` 或 `Form_DTitlebar` | DTitlebar | Form | 取 objectName 或 "DTitlebar" | accessible.h:82 |
| `Button_<objectName>` 或 `Button_DToolButton` | DToolButton | Button | 取 objectName 或 "DToolButton" | accessible.h:83 |
| `Form_<objectName>` 或 `Form_DDialog` | DDialog | Form | 取 objectName 或 "DDialog" | accessible.h:84 |
| `Form_<objectName>` 或 `Form_DFileDialog` | DFileDialog | Form | 取 objectName 或 "DFileDialog" | accessible.h:85 |

## 2. 显式 setAccessibleName 控件

| 预期 AT-SPI 名称 | 控件 | 角色 | 设置值 | 文件:行 |
|-----------------|------|------|--------|---------|
| `Button_ThumbnailToggle` | DIconButton (缩略图切换) | Button | "Button_ThumbnailToggle" | TitleWidget.cpp:22 |
| `Button_thumbnail` | DToolButton (缩略图) | Button | "Button_thumbnail" | SheetSidebar.cpp:363 |
| `Button_catalog` | DToolButton (目录) | Button | "Button_catalog" | SheetSidebar.cpp:363 |
| `Button_bookmark` | DToolButton (书签) | Button | "Button_bookmark" | SheetSidebar.cpp:363 |
| `Button_annotation` | DToolButton (注释) | Button | "Button_annotation" | SheetSidebar.cpp:363 |
| `Button_search` | DToolButton (搜索) | Button | "Button_search" | SheetSidebar.cpp:363 |
| `Menu_Browser` | BrowserMenu | Form | "Menu_Browser" | BrowserMenu.cpp:19 |
| `Menu_Hand` | HandleMenu | Form | "Menu_Hand" | TitleMenu.cpp:43 |
| `Menu_Note` | DMenu (注释右键) | Form | "Menu_Note" | SideBarImageListview.cpp:276 |
| `Menu_BookMark` | DMenu (书签右键) | Form | "Menu_BookMark" | SideBarImageListview.cpp:304 |
| `Label_TotalPage` | DLabel (总页数) | StaticText | "Label_TotalPage" | PagingWidget.cpp:74 |
| `Page` | DLineEdit (跳转页) | Editable | "Page" | PagingWidget.cpp:81 |
| `pageEdit` | QLineEdit (跳转页子编辑) | Editable | "pageEdit" | PagingWidget.cpp:84 |
| `Button_ThumbnailPre` | DIconButton (上一页) | Button | "Button_ThumbnailPre" | PagingWidget.cpp:100 |
| `Button_ThumbnailNext` | DIconButton (下一页) | Button | "Button_ThumbnailNext" | PagingWidget.cpp:107 |
| `CurrentPage` | DLabel (当前页) | StaticText | "CurrentPage" | PagingWidget.cpp:112 |
| `Form_findSearchEdit_P` | DSearchEdit (查找输入) | Form | "Form_findSearchEdit_P" | FindWidget.cpp:120 |
| `DLineEditChildLineEdit` | QLineEdit (查找子编辑) | Editable | "DLineEditChildLineEdit" | FindWidget.cpp:122 |
| `Label_Drag documents here` | DLabel (拖拽提示) | StaticText | "Label_Drag documents here" | CentralNavPage.cpp:23 |
| `Label_format supported: PDF,DJVU,DOCX` | DLabel (格式支持) | StaticText | (含 XPS 时后缀 XPS) | CentralNavPage.cpp:37 |
| `SelectFile` | DSuggestButton (选择文件) | Button | "SelectFile" | CentralNavPage.cpp:44 |
| `Label_Icon` | DLabel (图标) | StaticText | "Label_Icon" | CentralNavPage.cpp:58 |

## 3. 显式 setObjectName 控件（fallback 输入）

objectName 影响 AT 名仅当该类有 `accessibleFactory` 注册且 `getAccessibleName()` 的 `accessibleName` 为空。

| 控件 | ObjectName | 影响 AT 名 | 文件:行 |
|------|-----------|-----------|---------|
| DIconButton (缩略图) | "Thumbnails" | 通过 QPushButton→DIconButton 注册 → `Button_Thumbnails` | TitleWidget.cpp:21 |
| DLineEdit (缩放编辑) | "scaleEdit_P" | 通过 DLineEdit→DWidget 注册 → `Form_scaleEdit_P` | ScaleWidget.cpp:42 |
| QLineEdit (缩放编辑子) | "scaleEdit" | 通过 QLineEdit→QWidget 注册 → `Form_scaleEdit` | ScaleWidget.cpp:43 |
| DIconButton (缩小) | "SP_DecreaseElement" | 通过 DIconButton 注册 → `Button_SP_DecreaseElement` | ScaleWidget.cpp:72 |
| DIconButton (放大) | "SP_IncreaseElement" | 通过 DIconButton 注册 → `Button_SP_IncreaseElement` | ScaleWidget.cpp:79 |
| DIconButton (编辑箭头) | "editArrowBtn" | 通过 DIconButton 注册 → `Button_editArrowBtn` | ScaleWidget.cpp:52 |
| DSearchEdit (查找) | "findSearchEdit_P" | 通过 DSearchEdit 注册 → `Form_findSearchEdit_P` | FindWidget.cpp:119 |
| DIconButton (查找上) | "SP_ArrowUpBtn" | 通过 DIconButton 注册 → `Button_SP_ArrowUpBtn` | FindWidget.cpp:129 |
| DIconButton (查找下) | "SP_ArrowDownBtn" | 通过 DIconButton 注册 → `Button_SP_ArrowDownBtn` | FindWidget.cpp:136 |
| DDialogCloseButton (查找关) | "closeButton" | 通过 DDialogCloseButton→DIconButton 注册 → `Button_closeButton` | FindWidget.cpp:143 |
| DLineEdit (跳转页) | "Edit_Page_P" | 通过 DLineEdit→DWidget 注册 → `Form_Edit_Page_P` | PagingWidget.cpp:82 |
| QLineEdit (跳转页子) | "Edit_Page" | 通过 QLineEdit→QWidget 注册 → `Form_Edit_Page` | PagingWidget.cpp:83 |
| DIconButton (上页) | "thumbnailPreBtn" | 通过 DIconButton 注册 → `Button_thumbnailPreBtn` | PagingWidget.cpp:101 |
| DIconButton (下页) | "thumbnailNextBtn" | 通过 DIconButton 注册 → `Button_thumbnailNextBtn` | PagingWidget.cpp:108 |
| DSuggestButton (选文件) | "SelectFileBtn" | 通过 DSuggestButton→DPushButton 注册 → `Button_SelectFileBtn` | CentralNavPage.cpp:43 |
| DLabel (图标) | "iconSvg" | 通过 DLabel 注册 → `Label_iconSvg` | CentralNavPage.cpp:57 |
| DToolButton (切换侧栏) | "thumbnail" | 通过 DToolButton 注册 → `Button_thumbnail` | SheetSidebar.cpp:363 |
| DToolButton (目录) | "catalog" | 通过 DToolButton 注册 → `Button_catalog` | SheetSidebar.cpp:363 |
| DToolButton (书签) | "bookmark" | 通过 DToolButton 注册 → `Button_bookmark` | SheetSidebar.cpp:363 |
| DToolButton (注释) | "annotation" | 通过 DToolButton 注册 → `Button_annotation` | SheetSidebar.cpp:363 |
| DToolButton (搜索) | "search" | 通过 DToolButton 注册 → `Button_search` | SheetSidebar.cpp:363 |
| TextEditShadowWidget | "TextEditShadowWidget" | 通过 DWidget 注册 → `Form_TextEditShadowWidget` | TextEditWidget.cpp:52 |
| TransparentTextEdit | "TransparentTextEdit" | 通过 QWidget 注册 → `Form_TransparentTextEdit` | TransparentTextEdit.cpp:26 |
| SideBarImageListView | "ThumbnailListView" / "List_BookMark" / "List_Notes" / "SearchResult" | 通过 DListView 注册 → `Form_<objectName>` | SideBarImageListview.cpp:29,31 |
| EncryptionPage passwordEdit | "passwdEdit" | 通过 QLineEdit→QWidget → `Form_passwdEdit` | EncryptionPage.cpp:46 |
| EncryptionPage ensureBtn | "ensureBtn" | 通过 DPushButton 注册 → `Button_ensureBtn` | EncryptionPage.cpp:50 |
| QAction (菜单) | "New window" ... | 无 QWidget 注册 | 菜单项通过 QAction 对象名 |

## 4. 无 AT 注册 + 无显式名控件（fallback 为类型名 + 翻译文本）

| 控件 | 文件:行 | 当前 AT 名 | 风险 |
|------|---------|-----------|------|
| ScaleMenu | ScaleMenu.h:17 | 注册为 DMenu → `Form_<objectName>` 或 `Form_DMenu` | 无 objectName → 泛称 |
| HandleMenu | HandleMenu.h:17 | 同上，但显式设了 setAccessibleName | **已覆盖** |
| CatalogTreeView | CatalogTreeView.h:24 | 无 DTK 注册 → 系统默认名 | 查询子项困难 |
| TipsWidget | TipsWidget.h:15 | 注册为 DWidget → `Form_<objectName>` 或 `Form_widget` | 无 objectName → 泛称 |
| SecurityDialog | SecurityDialog.h:21 | 注册为 DDialog → `Form_<objectName>` 或 `Form_DDialog` | 无 objectName → 泛称 |
| ProgressDialog | ProgressDialog.h:11 | 注册为 DDialog → `Form_<objectName>` 或 `Form_DDialog` | 无 objectName → 泛称 |
| FileAttrWidget | FileAttrWidget.h:22 | DAbstractDialog → DDialog → `Form_<objectName>` 或 `Form_DDialog` | 无 objectName → 泛称 |
| RestoreTipWidget | RestoreTipWidget.h:23 | 注册为 DWidget → `Form_<objectName>` 或 `Form_widget` | 无 objectName → 泛称 |
| SlideWidget | SlideWidget.h:23 | 注册为 DWidget → `Form_<objectName>` 或 `Form_widget` | 无 objectName → 泛称 |
| SlidePlayWidget | SlidePlayWidget.h:21 | 注册为 DFloatingButton → `Button_<toolTip>` 或 `Button_DFloatingButton` | 无 toolTip → 泛称 |
| BrowserMagniFier | BrowserMagniFier.h:58 | 注册为 QLabel → `Label_<objectName>` 或 `Label_DLabel` | 无 objectName → 泛称 |
| LoadingWidget | DocSheet.h:910 | 注册为 QWidget → `Form_<objectName>` 或 `Form_widget` | 无 objectName → 泛称 |
| TextEditWidget | TextEditWidget.h:23 | 通过 BaseWidget 注册 → `Form_<objectName>` 或 `Form_widget` | 无 objectName → 泛称 |
| ColorWidgetAction | ColorWidgetAction.h | QWidgetAction → 非 QWidget | 不受 accessibleFactory 影响 |

## 5. 动态 AT 名（需运行时验证）

以下控件 AT 名中可能包含 `_N` 后缀（同名多实例）：

| 场景 | 说明 |
|------|------|
| 多文档标签页 | 每个 DocTab 的文字标签相同 → 可能后缀 `_1`、`_2` |
| 多 MainWindow 窗口 | 多窗口时每个 MainWindow 同名 → 后缀 `_1`、`_2` |
| 多 DocSheet | 每个 DSplitter 同名 Form_DocSheet → 后缀 `_1` 、`_2` |
| 同一 Sidebar 中同一类型按钮 | DToolButton 同名 → 已验证通过不同 objectName 区分 |

## 6. 菜单项和 Action

QAction 不直接呈现为 AT-SPI 子元素——它们通过菜单的 DMenu 子项暴露。菜单项可通过 objectName 定位：

| 菜单 | 关键 objectName 列表 |
|------|-------------------|
| TitleMenu | "New window", "New tab", "Save", "Save as", "Display in file manager", "Magnifer", "Search", "Print" |
| HandleMenu | "defaultshape", "handleshape" |
| BrowserMenu | 见 ui-map.md 完整列表 |
| NoteMenu | 无 objectName，通过动作文本定位 |
| BookMarkMenu | 同上 |
| ScaleMenu | 无 objectName，通过动作文本定位 |

## 7. 推导链汇总

```
AT-SPI 名称 = getAccessibleName(widget, role, fallback) 算法:
  1. 若 widget->accessibleName() 非空 → RolePrefix + "_" + accessibleName
  2. 否则 → RolePrefix + "_" + fallback（宏参数）
  3. 冲突 → 追加 "_N"

角色前缀: Form_ / Button_ / Label_ / Editable_ / Slider_

应用级注册（accessible.h:49-62）: 固定 fallback 名
通用控件注册（accessible.h:65-85）: 优先取 objectName
显式 setAccessibleName: 覆盖 accessibleName，优先于 objectName
显式 setObjectName: 被 accessibleName 为空时作为 fallback 使用