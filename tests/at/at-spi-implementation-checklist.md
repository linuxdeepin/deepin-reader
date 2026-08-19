# deepin-reader AT-SPI 实现缺口检查清单

## 命名规则

所有新加 `setAccessibleName` 遵循：`<命名约定>.<控件类型>.<功能>` 模式。

推荐前缀约定：
- `DeepinReader` — 顶级命名空间，避免与其他应用冲突

## 已完成的 AT-SPI 注册

### 类级注册（accessibleFactory → 全部完备）

以下类已通过 `accessibleFactory` 注册 AT-SPI 接口：
✅ MainWindow, Central, CentralDocPage, CentralNavPage,
✅ DocSheet, DocTabBar, TitleWidget, SheetSidebar, SheetBrowser,
✅ ThumbnailWidget, CatalogWidget, BookMarkWidget, NotesWidget, SearchResWidget,
✅ QFrame, QWidget, QPushButton, QSlider, DFrame, DWidget,
✅ DBackgroundGroup, DSwitchButton, DFloatingButton, DSearchEdit,
✅ DPushButton, DIconButton, DCheckBox, DCommandLinkButton,
✅ DTitlebar, DDialog, DFileDialog

### 已设置 AccessibleName 的控件

| 控件 | 名称 | 文件:行 | 状态 |
|------|------|---------|------|
| MainWindow | DocMainWindow | MainWindow.cpp:620 | ✅ |
| TitleWidget → 标题 | ReaderTitle | TitleWidget.cpp:22 | ✅ |
| TitleWidget → 主菜单按钮 | ReaderMenu | TitleWidget.cpp:22 | ✅ |
| CentralNavPage → 添加文件 | AddFileBtn | CentralNavPage.cpp:23 | ✅ |
| CentralNavPage → 打开文件 | OpenFileBtn | CentralNavPage.cpp:37 | ✅ |
| CentralNavPage → 最近文件 | RecentView | CentralNavPage.cpp:44 | ✅ |
| CentralNavPage → 文件项 | FileItem | CentralNavPage.cpp:58 | ✅ |
| SheetBrowser → 页面 | BrowserPage | SheetBrowser.cpp:90 | ✅ |
| SheetBrowser → 上一页 | PrevPage | SheetBrowser.cpp:101 | ✅ |
| BrowserMenu → 右键菜单 | BrowserMenu | BrowserMenu.cpp:19 | ✅ |
| ThumbnailWidget → 缩略图 | ThumbnailView | ThumbnailWidget.cpp:38 | ✅ |
| ThumbnailWidget → 列表 | SidebarListView | ThumbnailWidget.cpp:43 | ✅ |
| ThumbnailWidget → 页码标签 | PageLabel | ThumbnailWidget.cpp:50 | ✅ |
| CatalogWidget → 目录树 | CatalogTree | CatalogWidget.cpp:40 | ✅ |
| CatalogWidget → 列表 | SidebarListView | CatalogWidget.cpp:55 | ✅ |
| BookMarkWidget → 书签列表 | BookMarkTree | BookMarkWidget.cpp:45 | ✅ |
| BookMarkWidget → 书签编辑 | BookMarkEdit | BookMarkWidget.cpp:52 | ✅ |
| BookMarkWidget → 无书签提示 | NoBookMark | BookMarkWidget.cpp:68 | ✅ |
| NotesWidget → 注释列表 | NotesView | NotesWidget.cpp:46 | ✅ |
| NotesWidget → 注释编辑 | NotesEdit | NotesWidget.cpp:55 | ✅ |
| NotesWidget → 无注释提示 | NoNotes | NotesWidget.cpp:66 | ✅ |
| SearchResWidget → 搜索 | SearchResult | SearchResWidget.cpp:43 | ✅ |
| SheetSidebar → 侧栏按钮 | LeftBtn | SheetSidebar.cpp:363 | ✅ (多实例 _N) |
| SideBarImageListView → 注释菜单 | PopupNoteMenu | SideBarImageListview.cpp:276 | ✅ |
| SideBarImageListView → 书签菜单 | PopupBookMarkMenu | SideBarImageListview.cpp:304 | ✅ |
| FindWidget → 查找输入 | FindInput | FindWidget.cpp:120 | ✅ |
| PagingWidget → 页码 | PageCount | PagingWidget.cpp:74 | ✅ |
| PagingWidget → 当前页 | CurrentPage | PagingWidget.cpp:81 | ✅ |
| PagingWidget → 滑块 | ReaderSlider | PagingWidget.cpp:84 | ✅ |
| PagingWidget → 缩小 | ReaderSliderZoomIn | PagingWidget.cpp:100 | ✅ |
| PagingWidget → 放大 | ReaderSliderZoomOut | PagingWidget.cpp:107 | ✅ |
| PagingWidget → 搜索 | SearchButton | PagingWidget.cpp:112 | ✅ |
| SlidePlayWidget → 按钮 | SlidePre/SlideNext/SlidePlay/SlideClose | SlidePlayWidget.cpp:160 | ✅ |
| TitleMenu | ReaderMenu | TitleMenu.cpp:43 | ✅ |

---

## 缺口分析

### 🔴 必做（无任何 AT 锚点或 fallback 不可靠）

| # | 目标控件 | 缺什么 | 文件 | 风险说明 |
|---|---------|--------|------|---------|
| G1 | DocTabBar 标签项 | setAccessibleName | `reader/uiframe/DocTabBar.cpp` | fallback 为翻译文本"×关闭" |
| G2 | FindWidget 查找按钮（上/下/关闭） | setAccessibleName | `reader/widgets/FindWidget.cpp:129,136,143` | 只用 objectName，缺少 AT 可见名 |
| G3 | ScaleWidget 放大/缩小/缩放菜单 | setAccessibleName | `reader/widgets/ScaleWidget.cpp:42,43,52,72` | 只设 objectName，fallback 为翻译文本 |
| G4 | ScaleMenu 缩放比例菜单项 | setAccessibleName | `reader/widgets/ScaleMenu.cpp` | 缩放比例选项需 AT 可定位 |
| G5 | HandleMenu 工具切换菜单项 | setAccessibleName | `reader/widgets/HandleMenu.cpp` | "选择工具"/"手形工具"需 AT 名 |
| G6 | SlideWidget 幻灯片视图 | setAccessibleName | `reader/widgets/SlideWidget.cpp` | 幻灯片模式缺少 AT 标识 |
| G7 | EncryptionPage 加密输入框/按钮 | setAccessibleName | `reader/widgets/EncryptionPage.cpp:46,50` | 只设 objectName |
| G8 | ProgressDialog 进度对话框 | setAccessibleName | `reader/widgets/ProgressDialog.cpp` | 加载中状态无可信 AT 锚点 |
| G9 | SaveDialog 保存/退出对话框 | setAccessibleName | `reader/widgets/SaveDialog.cpp` | 保存确认按钮缺 AT 名 |
| G10 | SecurityDialog 安全提示 | setAccessibleName | `reader/widgets/SecurityDialog.cpp` | 安全确认按钮缺 AT 名 |
| G11 | FileAttrWidget 文件属性 | setAccessibleName | `reader/widgets/FileAttrWidget.cpp` | 属性窗口内部控件缺 AT 名 |
| G12 | TextEditWidget/TransparentTextEdit | setAccessibleName | `reader/widgets/TextEditWidget.cpp` | 文本编辑模式缺 AT 标识 |
| G13 | DocSheet 打开时进度/状态 | setAccessibleName | `reader/uiframe/DocSheet.cpp` | 文档加载状态缺少 AT 锚点 |
| G14 | ColorWidgetAction 颜色选择 | setAccessibleName | `reader/widgets/ColorWidgetAction.cpp` | 只设 objectName |
| G15 | EyeProtectionAction 护眼模式 | setAccessibleName | `reader/eyeprotection/EyeProtectionAction.cpp` | 只设 objectName |

### 🟡 建议（fallback 易碎）

| # | 目标控件 | 当前 fallback | 文件 | 建议 |
|---|---------|---------------|------|------|
| G16 | SheetBrowser 滚动区域 | 翻译文本 | `SheetBrowser.cpp:98` | 添加 setAccessibleName("ReaderScrollArea") |
| G17 | TitleWidget 搜索按钮标签 | "Search Btn" objectName | `TitleWidget.cpp:21` | setAccessibleName 已设? 需运行时确认 |
| G18 | CentralDocPage 各子 widget | QWidget fallback | `CentralDocPage.cpp` | 英文类名作为 fallback 不稳定 |

### 🟢 无需修改（已有注册/显式名）

所有已在 ✅ 表格中的控件无需修改。

---

## 实施模板

### G1: DocTabBar 标签项

```cpp
// reader/uiframe/DocTabBar.cpp — 在标签创建位置添加
// 在 addTab / setTabText 后
int index = addTab(text);
QWidget *tabBarButton = tabBar()->tabButton(index, QTabBar::LeftSide);
if (tabBarButton) {
    tabBarButton->setAccessibleName(QString("DocTabItem_%1").arg(index));
}
```

### G2: FindWidget 查找按钮

```cpp
// reader/widgets/FindWidget.cpp — 在 initWidget 中
// 定位: 约 line 129 — "查找上一个"按钮
m_findPrevBtn = new QPushButton(tr("查找上一个"));
m_findPrevBtn->setAccessibleName("FindPrevBtn");

// 定位: 约 line 136 — "查找下一个"按钮
m_findNextBtn = new QPushButton(tr("查找下一个"));
m_findNextBtn->setAccessibleName("FindNextBtn");

// 定位: 约 line 143 — "关闭"按钮
m_findCloseBtn = new QPushButton();
m_findCloseBtn->setAccessibleName("FindCloseBtn");
```

### G3: ScaleWidget 缩放按钮

```cpp
// reader/widgets/ScaleWidget.cpp — 在 initWidget 中
// 定位: 约 line 42 — ZoomIn
m_zoomInBtn->setAccessibleName("ReaderZoomIn");
// 定位: 约 line 43 — ScaleComboBox
m_scaleCombo->setAccessibleName("ReaderScaleCombo");
// 定位: 约 line 52 — ZoomOut
m_zoomOutBtn->setAccessibleName("ReaderZoomOut");
```

### G7: EncryptionPage 加密控件

```cpp
// reader/widgets/EncryptionPage.cpp — 在 InitUI 中
// 定位: 约 line 46
ownerPasswordEdit->setAccessibleName("OwnerPasswordEdit");
// 定位: 约 line 50
userPasswordEdit->setAccessibleName("UserPasswordEdit");
```

---

## 验证断言

| 缺口 | 验证断言（AT 用例如何定位） |
|------|---------------------------|
| G1 | `assert_element → selector: {name: "DocTabItem_0", role: "push button"}` |
| G2 | `assert_element → selector: {name: "FindPrevBtn", role: "push button"}` |
| G3 | `assert_element → selector: {name: "ReaderZoomIn", role: "push button"}` |
| G7 | `assert_element → selector: {name: "OwnerPasswordEdit", role: "entry"}` |

---

## 覆盖率统计

| 指标 | 数值 |
|------|------|
| 已注册类级 AT 接口 | 34 类 (accessibleFactory) |
| 已显式 setAccessibleName 调用点 | 30+ 处 |
| 必做缺口数 (🔴) | 15 |
| 建议缺口数 (🟡) | 3 |
| 无需修改数 (🟢) | 30+ |
| 覆盖率估计 (已覆盖/应有) | ~70% |

---

## 动态风险

- `_N` 去重编号：同名控件（如 LeftBtn_0~4）编号取决于创建顺序，不可作稳定 AT 锚点
- QGraphicsItem（BrowserPage, BrowserAnnotation）：无独立 AT-SPI 接口，需通过父容器定位
- 翻译文本 fallback：未设 accessibleName 的控件在系统语言切换后 AT 名变化
- DMenu（TitleMenu, BrowserMenu）：瞬态元素，仅弹出时出现在 AT-SPI 树中

## 实施后验证步骤

1. **编译验证**: 每处修改后 `cd build && cmake .. && make -j$(nproc)` → 退出码 0
2. **冒烟测试**: 运行应用，确认可启动、可打开文档
3. **运行时对账**: 使用 `accerciser` 或 `python3 -c "import pyatspi; ..."` 检查新 AT 名是否存在
4. **静态计数**: 确认新增 setAccessibleName 调用数与计划一致