# deepin-reader AT-SPI 实施清单

## 命名约定

```
<App>.<Widget> — 例如: DeepinReader.MainWindow, DeepinReader.Sidebar
```

所有新增 accessibleName 使用英文，稳定且不含翻译文本。

## 现有注册覆盖统计

### 通过 accessible.h 覆盖的控件类型数

| 注册级别 | 控件类型数 | 示例 |
|---------|-----------|------|
| 应用级 (SET_FORM_ACCESSIBLE 固定名) | 14 | MainWindow, Central, DocSheet, ... |
| 通用 Qt 控件 | 4 | QFrame, QWidget, QPushButton, QSlider |
| 通用 DTK 控件 | 14 | DFrame, DWidget, DBackgroundGroup, DSwitchButton, DFloatingButton, DSearchEdit, DPushButton, DIconButton, DCheckBox, DCommandLinkButton, DLabel, DTitlebar, DToolButton, DDialog, DFileDialog |
| **未覆盖的 DTK/Qt 控件类型** | **2** | **DTreeView, DMenu** |

### 显式 setAccessibleName 调用点

| 位置 | 值 | 对象 |
|------|----|------|
| TitleWidget.cpp:22 | "Button_ThumbnailToggle" | DIconButton (缩略图切换) |
| SheetSidebar.cpp:363 | "Button_<objName>" | DToolButton (侧栏切换按钮) |
| BrowserMenu.cpp:19 | "Menu_Browser" | BrowserMenu |
| TitleMenu.cpp:43 | "Menu_Hand" | HandleMenu (作为子菜单) |
| SideBarImageListview.cpp:276 | "Menu_Note" | DMenu (注释右键) |
| SideBarImageListview.cpp:304 | "Menu_BookMark" | DMenu (书签右键) |
| PagingWidget.cpp:74 | "Label_TotalPage" | DLabel |
| PagingWidget.cpp:81 | "Page" | DLineEdit |
| PagingWidget.cpp:84 | "pageEdit" | QLineEdit |
| PagingWidget.cpp:100 | "Button_ThumbnailPre" | DIconButton |
| PagingWidget.cpp:107 | "Button_ThumbnailNext" | DIconButton |
| PagingWidget.cpp:112 | "CurrentPage" | DLabel |
| FindWidget.cpp:120 | "Form_findSearchEdit_P" | DSearchEdit |
| FindWidget.cpp:122 | "DLineEditChildLineEdit" | QLineEdit |
| CentralNavPage.cpp:23 | "Label_Drag documents here" | DLabel |
| CentralNavPage.cpp:37 | "Label_format supported: ..." | DLabel |
| CentralNavPage.cpp:44 | "SelectFile" | DSuggestButton |
| CentralNavPage.cpp:58 | "Label_Icon" | DLabel |

### 显式 setObjectName 调用点（影响 AT fallback 名）

| 文件 | 对象 | ObjectName | 推导 AT 名 |
|------|------|-----------|-----------|
| TitleWidget.cpp:21 | DIconButton | "Thumbnails" | Button_Thumbnails |
| ScaleWidget.cpp:42 | DLineEdit | "scaleEdit_P" | Form_scaleEdit_P |
| ScaleWidget.cpp:43 | QLineEdit | "scaleEdit" | Form_scaleEdit |
| ScaleWidget.cpp:72 | DIconButton | "SP_DecreaseElement" | Button_SP_DecreaseElement |
| ScaleWidget.cpp:79 | DIconButton | "SP_IncreaseElement" | Button_SP_IncreaseElement |
| ScaleWidget.cpp:52 | DIconButton | "editArrowBtn" | Button_editArrowBtn |
| FindWidget.cpp:119 | DSearchEdit | "findSearchEdit_P" | 已覆盖 |
| FindWidget.cpp:129 | DIconButton | "SP_ArrowUpBtn" | Button_SP_ArrowUpBtn |
| FindWidget.cpp:136 | DIconButton | "SP_ArrowDownBtn" | Button_SP_ArrowDownBtn |
| FindWidget.cpp:143 | DDialogCloseButton | "closeButton" | Button_closeButton |
| PagingWidget.cpp:82 | DLineEdit | "Edit_Page_P" | Form_Edit_Page_P |
| PagingWidget.cpp:83 | QLineEdit | "Edit_Page" | Form_Edit_Page |
| PagingWidget.cpp:101 | DIconButton | "thumbnailPreBtn" | Button_thumbnailPreBtn |
| PagingWidget.cpp:108 | DIconButton | "thumbnailNextBtn" | Button_thumbnailNextBtn |
| CentralNavPage.cpp:43 | DSuggestButton | "SelectFileBtn" | 已覆盖 |
| CentralNavPage.cpp:57 | DLabel | "iconSvg" | Label_iconSvg |
| EncryptionPage.cpp:46 | QLineEdit | "passwdEdit" | Form_passwdEdit |
| EncryptionPage.cpp:50 | DPushButton | "ensureBtn" | Button_ensureBtn |
| SideBarImageListview.cpp:29,31 | DListView | "ThumbnailListView" / "List_BookMark" / "List_Notes" / "SearchResult" | Form_<name> |
| TextEditWidget.cpp:52 | DWidget | "TextEditShadowWidget" | Form_TextEditShadowWidget |
| TransparentTextEdit.cpp:26 | QWidget | "TransparentTextEdit" | Form_TransparentTextEdit |
| SheetBrowser.cpp:90,98,101 | DGraphicsView | 子控件 | 已覆盖 |

---

## 缺口清单

### GAP-01: CatalogTreeView (DTreeView) — 无 AT 注册，无显式名称

**目标名称**: `DeepinReader.CatalogTree`

**问题**: CatalogTreeView 继承 DTreeView，但 accessible.h 未注册 DTreeView。缺 setAccessibleName 和 setObjectName。

**插入位置**: `reader/sidebar/CatalogTreeView.cpp` 构造函数结尾（约第 30 行后）

**修改模板**:
```cpp
// 在 CatalogTreeView 构造函数末尾添加
setObjectName("CatalogTree");
setAccessibleName("Tree_Catalog");
```

**验证断言**: AT-SPI 查询应定位到 `Form_Tree_Catalog` 或 `Tree_Catalog`（取决于 AT 注册是否覆盖，实际为默认 QAccessibleWidget → accessibleName 直接生效）。

**注意**: 由于 DTreeView 不在 accessible.h 中，`setAccessibleName` 直接生效（Qt 默认 QAccessibleWidget 会使用 accessibleName）。建议同时添加 accessible.h 注册条目。

---

### GAP-02: ScaleMenu (DMenu) — 无显式名称

**目标名称**: `DeepinReader.ScaleMenu`

**问题**: ScaleMenu 继承 DMenu，无 setAccessibleName 和 setObjectName。DMenu 不在 accessible.h 注册表中。

**插入位置**: `reader/widgets/ScaleMenu.cpp` 构造函数

**修改模板**:
```cpp
// 在 ScaleMenu 构造函数末尾添加
setAccessibleName("Menu_Scale");
```

**验证断言**: AT-SPI 查询应定位到 accessibleName="Menu_Scale" 的菜单。

---

### GAP-03: TipsWidget (DWidget) — 无显式名称

**目标名称**: `DeepinReader.TipsWidget`

**问题**: TipsWidget 继承 DWidget，有 accessible.h 注册（DWidget → `Form_widget` 或 objectName），但缺 setAccessibleName 和 setObjectName。多实例时 AT 名均为 `Form_widget`（或 `Form_widget_1`、`Form_widget_2` 等不可控后缀）。

**插入位置**: `reader/widgets/TipsWidget.cpp` 构造函数

**修改模板**:
```cpp
// 在 TipsWidget 构造函数中 initWidget 调用后或结尾添加
setObjectName("TipsWidget");
setAccessibleName("Widget_Tips");
```

**验证断言**: AT-SPI 查询应定位到 `Form_Widget_Tips`。

---

### GAP-04: SecurityDialog (DDialog) — 无显式名称

**目标名称**: `DeepinReader.SecurityDialog`

**问题**: SecurityDialog 继承 DDialog，有 accessible.h 注册。但无 setAccessibleName 和 setObjectName，AT 名 fallback 为 `Form_DDialog`。

**插入位置**: `reader/widgets/SecurityDialog.cpp` 构造函数

**修改模板**:
```cpp
// 在 SecurityDialog 构造函数末尾添加
setObjectName("SecurityDialog");
setAccessibleName("Dialog_Security");
```

**验证断言**: AT-SPI 查询应定位到 `Form_Dialog_Security`。

---

### GAP-05: ProgressDialog (DDialog) — 无显式名称

**目标名称**: `DeepinReader.ProgressDialog`

**问题**: ProgressDialog 继承 DDialog，有 accessible.h 注册。但无 setAccessibleName 和 setObjectName，AT 名 fallback 为 `Form_DDialog`。

**插入位置**: `reader/widgets/ProgressDialog.cpp` 构造函数

**修改模板**:
```cpp
// 在 ProgressDialog 构造函数末尾添加
setObjectName("ProgressDialog");
setAccessibleName("Dialog_Progress");
```

**验证断言**: AT-SPI 查询应定位到 `Form_Dialog_Progress`。

---

### GAP-06: FileAttrWidget (DAbstractDialog) — 无显式名称

**目标名称**: `DeepinReader.FileAttrWidget`

**问题**: FileAttrWidget 继承 DAbstractDialog→DDialog，有 accessible.h 注册。但无 setAccessibleName 和 setObjectName，AT 名 fallback 为 `Form_DDialog`。

**插入位置**: `reader/widgets/FileAttrWidget.cpp` 构造函数

**修改模板**:
```cpp
// 在 FileAttrWidget 构造函数末尾添加
setObjectName("FileAttrWidget");
setAccessibleName("Dialog_FileAttr");
```

**验证断言**: AT-SPI 查询应定位到 `Form_Dialog_FileAttr`。

---

### GAP-07: RestoreTipWidget (DWidget) — 无显式名称

**目标名称**: `DeepinReader.RestoreTipWidget`

**问题**: RestoreTipWidget 继承 DWidget，有 accessible.h 注册。但无 setAccessibleName 和 setObjectName。

**插入位置**: `reader/widgets/RestoreTipWidget.cpp` 构造函数

**修改模板**:
```cpp
// 在 RestoreTipWidget 构造函数末尾添加
setObjectName("RestoreTipWidget");
setAccessibleName("Widget_RestoreTip");
```

**验证断言**: AT-SPI 查询应定位到 `Form_Widget_RestoreTip`。

---

### GAP-08: SlideWidget (DWidget) — 无显式名称

**目标名称**: `DeepinReader.SlideWidget`

**问题**: SlideWidget 继承 DWidget，有 accessible.h 注册。但无 setAccessibleName 和 setObjectName。

**插入位置**: `reader/widgets/SlideWidget.cpp` 构造函数

**修改模板**:
```cpp
// 在 SlideWidget 构造函数末尾添加
setObjectName("SlideWidget");
setAccessibleName("Widget_Slide");
```

**验证断言**: AT-SPI 查询应定位到 `Form_Widget_Slide`。

---

### GAP-09: SlidePlayWidget (DFloatingWidget) — 无显式名称

**目标名称**: `DeepinReader.SlidePlayWidget`

**问题**: SlidePlayWidget 继承 DFloatingWidget，有 accessible.h 注册（归类为 DFloatingButton → `Button_<toolTip>` 或 `Button_DFloatingButton`）。但无 toolTip/setAccessibleName/setObjectName。

**插入位置**: `reader/widgets/SlidePlayWidget.cpp` 构造函数

**修改模板**:
```cpp
// 在 SlidePlayWidget 构造函数中 createBtn 后或末尾添加
setObjectName("SlidePlayWidget");
setAccessibleName("Widget_SlidePlay");
```

**验证断言**: AT-SPI 查询应定位到 `Button_Widget_SlidePlay` 或 `Form_Widget_SlidePlay`（取决于实际 AT 角色）。

---

### GAP-10: BrowserMagniFier (QLabel) — 无显式名称

**目标名称**: `DeepinReader.BrowserMagniFier`

**问题**: BrowserMagniFier 继承 QLabel，有 accessible.h 注册（QLabel→`Label_<objectName>` 或 `Label_DLabel`）。但无 setAccessibleName 和 setObjectName。

**插入位置**: `reader/browser/BrowserMagniFier.cpp` 构造函数

**修改模板**:
```cpp
// 在 BrowserMagniFier 构造函数末尾添加
setObjectName("BrowserMagniFier");
setAccessibleName("Label_Magnifier");
```

**验证断言**: AT-SPI 查询应定位到 `Label_Label_Magnifier`（注意：Label_ 前缀 + 值）。

---

### GAP-11: LoadingWidget (QWidget) — 无显式名称

**目标名称**: `DeepinReader.LoadingWidget`

**问题**: LoadingWidget 定义于 DocSheet.h:910，继承 QWidget，有 accessible.h 注册。但无 setAccessibleName 和 setObjectName。

**插入位置**: `reader/uiframe/DocSheet.cpp` 中 LoadingWidget 的构造函数体（约第 910 行附近）

**修改模板**:
```cpp
// 在 LoadingWidget 构造函数中添加
setObjectName("LoadingWidget");
setAccessibleName("Widget_Loading");
```

**验证断言**: AT-SPI 查询应定位到 `Form_Widget_Loading`。

---

### GAP-12: TextEditWidget (BaseWidget → DWidget) — 无显式名称

**目标名称**: `DeepinReader.TextEditWidget`

**问题**: TextEditWidget 继承 BaseWidget→DWidget，有 accessible.h 注册。但无 setAccessibleName 和 setObjectName。

**插入位置**: `reader/widgets/TextEditWidget.cpp` 构造函数

**修改模板**:
```cpp
// 在 TextEditWidget 构造函数末尾添加
setObjectName("TextEditWidget");
setAccessibleName("Widget_TextEdit");
```

**验证断言**: AT-SPI 查询应定位到 `Form_Widget_TextEdit`。

---

### GAP-13: ColorWidgetAction (QWidgetAction) — 无显式名称，非 QWidget 不受 accessibleFactory 影响

**目标名称**: `DeepinReader.ColorWidgetAction`

**问题**: ColorWidgetAction 继承 QWidgetAction。QWidgetAction 不是 QWidget，不受 accessibleFactory 注册影响。需通过内部 widget 的 accessibleName 控制。

**插入位置**: `reader/widgets/ColorWidgetAction.cpp` 构造函数

**修改模板**:
```cpp
// 在 ColorWidgetAction 构造函数中，对内部 widget 设置
if (auto *w = widget()) {
    w->setObjectName("ColorWidgetAction");
    w->setAccessibleName("Widget_ColorPicker");
}
```

**验证断言**: AT-SPI 查询应定位到 `Form_Widget_ColorPicker`。

---

### GAP-14: EyeProtectionAction (QWidgetAction) — 无 AT 名称

**目标名称**: `DeepinReader.EyeProtectionAction`

**问题**: EyeProtectionAction 继承 QWidgetAction。非 QWidget 不受 accessibleFactory 影响。

**插入位置**: `reader/eyeprotection/EyeProtectionAction.cpp` 构造函数

**修改模板**:
```cpp
// 在 EyeProtectionAction 构造函数末尾，对内部 widget 设置
if (auto *w = widget()) {
    w->setObjectName("EyeProtectionAction");
    w->setAccessibleName("Widget_EyeProtection");
}
```

**验证断言**: AT-SPI 查询应定位到 `Form_Widget_EyeProtection`。

---

## 无需修改的控件（已覆盖或需运行时验证）

| 控件 | 理由 |
|------|------|
| MainWindow | SET_FORM_ACCESSIBLE("MainWindow") → `Form_MainWindow` |
| Central | SET_FORM_ACCESSIBLE("Central") → `Form_Central` |
| CentralDocPage | SET_FORM_ACCESSIBLE("CentralDocPage") → `Form_CentralDocPage` |
| CentralNavPage | SET_FORM_ACCESSIBLE("CentralNavPage") + 子控件显式名 |
| DocSheet | SET_FORM_ACCESSIBLE("DocSheet") → `Form_DocSheet` |
| DocTabBar | SET_FORM_ACCESSIBLE("DocTabBar") → `Form_DocTabBar` |
| TitleWidget | SET_FORM_ACCESSIBLE("TitleWidget") + 子控件显式名 |
| SheetSidebar | SET_FORM_ACCESSIBLE("SheetSidebar") + 子控件显式名 |
| SheetBrowser | SET_FORM_ACCESSIBLE("SheetBrowser") + 子控件显式名 |
| ThumbnailWidget | SET_FORM_ACCESSIBLE("ThumbnailWidget") + 子控件显式名 |
| CatalogWidget | SET_FORM_ACCESSIBLE("CatalogWidget") + 子控件显式名 |
| BookMarkWidget | SET_FORM_ACCESSIBLE("BookMarkWidget") + 子控件显式名 |
| NotesWidget | SET_FORM_ACCESSIBLE("NotesWidget") + 子控件显式名 |
| SearchResWidget | SET_FORM_ACCESSIBLE("SearchResWidget") + 子控件显式名 |
| BrowserMenu | setAccessibleName("Menu_Browser") |
| HandleMenu | setAccessibleName("Menu_Hand") |
| TitleMenu | TitleMenu 本身是 DMenu；其子菜单 HandleMenu 有显式名 |
| FindWidget | 子控件均有 setAccessibleName/setObjectName |
| PagingWidget | 子控件均有 setAccessibleName |
| ScaleWidget | 子控件均有 setObjectName |
| EncryptionPage | 子控件有 setObjectName |
| SideBarImageListView | 有 setObjectName |
| NoteMenu / BookMarkMenu | setAccessibleName("Menu_Note") / ("Menu_BookMark") |
| SlidePlayWidget 子按钮 | createBtn 内部有 setObjectName |
| BrowserPage | QGraphicsItem 非 QWidget，不受 AT 注册影响 |

## 实施优先级

| 优先级 | 条目 | 理由 |
|--------|------|------|
| P0 | GAP-01 (CatalogTreeView) | DTreeView 无 AT 注册 + 无名称，AT 无法定位目录树 |
| P0 | GAP-02 (ScaleMenu) | DMenu 无 AT 注册 + 无名称，缩放菜单无法定位 |
| P1 | GAP-03 (TipsWidget) | 多实例提示控件，需区分 |
| P1 | GAP-04 (SecurityDialog) | 安全弹窗，关键交互点 |
| P1 | GAP-05 (ProgressDialog) | 进度对话框，进度测试需要 |
| P1 | GAP-06 (FileAttrWidget) | 文件属性对话框 |
| P2 | GAP-07 (RestoreTipWidget) | 恢复位置提示条 |
| P2 | GAP-08 (SlideWidget) | 幻灯片模式控件 |
| P2 | GAP-09 (SlidePlayWidget) | 幻灯片播放控件 |
| P2 | GAP-10 (BrowserMagniFier) | 放大镜控件 |
| P2 | GAP-11 (LoadingWidget) | 加载中控件 |
| P2 | GAP-12 (TextEditWidget) | 文本编辑控件 |
| P3 | GAP-13 (ColorWidgetAction) | QWidgetAction 非标准路径 |
| P3 | GAP-14 (EyeProtectionAction) | QWidgetAction 非标准路径 |

## 验证策略

1. **编译验证**: 每处修改后 `mkdir build && cd build && cmake .. && make -j$(nproc)` 确认编译通过，无新增 warning
2. **冒烟验证**: 运行 `deepin-reader` 打开一个 PDF 文件，确认 UI 正常
3. **AT-SPI dump 验证**: 使用 `atspi_dump.py` 或 `youqu at dump` 导出 AT-SPI 树，确认新增名称出现在正确位置
4. **静态计数勾对**: 确认 `grep -r "setAccessibleName" reader/ | wc -l` 增量与预期一致