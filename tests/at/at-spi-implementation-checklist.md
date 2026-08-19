# AT-SPI Implementation Checklist — deepin-reader

## 当前 AT-SPI 覆盖统计

- 源码 UI 类总数: 173（扫描发现）
- 有 setAccessibleName 的 UI 类: 8（含辅助方法中的调用）
- 无 AT-SPI 名称的 UI 类: 165
- 覆盖率: 4.6%

## 命名规则建议

```cpp
// 统一命名前缀约定
// 主窗口: <Widget>_<Function>_<Index>
// 示例:
// MainWindow_TitleMenu   — 主菜单按钮
// Central_DocTab         — 文档标签页
// SheetSidebar_Btn_01    — 侧边栏第一个按钮
// FindWidget_SearchInput — 查找输入框
```

## 缺口明细

### 必做（无任何锚点）

| # | 目标名称 | 当前 fallback | 文件位置 | 修改模板 |
|---|---------|--------------|---------|---------|
| 1 | `TitleMenu.TitleMenu` | 无 | `reader/uiframe/TitleMenu.cpp:43` | 在构造函数中加 `setAccessibleName("TitleMenu_MenuBtn")` |
| 2 | `Central.Central` | 无 | `reader/uiframe/Central.cpp` 构造函数 | 在构造函数中加 `setAccessibleName("Central_MainArea")` |
| 3 | `CentralDocPage.CentralDocPage` | 无 | `reader/uiframe/CentralDocPage.cpp` 构造函数 | 在构造函数中加 `setAccessibleName("CentralDocPage_DocArea")` |
| 4 | `DocSheet.DocSheet` | 无 | `reader/uiframe/DocSheet.cpp` 构造函数 | 在构造函数中加 `setAccessibleName("DocSheet_Container")` |
| 5 | `DocTabBar.DocTabBar` | 无 | `reader/uiframe/DocTabBar.cpp` 构造函数 | 在构造函数中加 `setAccessibleName("DocTabBar_Tabs")` |
| 6 | `FindWidget.FindWidget` | 无 | `reader/widgets/FindWidget.cpp:120` | 在 initWidget 中加 `m_findEdit->setAccessibleName("FindWidget_SearchInput")` |
| 7 | `SheetSidebar.SheetSidebar` | 无 | `reader/sidebar/SheetSidebar.cpp:363` | 在 createBtn 中加 `button->setAccessibleName("SheetSidebar_" + btn->text())` |
| 8 | `ScaleWidget.ScaleWidget` | 无 | `reader/widgets/ScaleWidget.cpp:42-79` | 对各子控件加 setAccessibleName |

### 建议（fallback 易碎）

| # | 目标名称 | 当前 fallback | 风险 | 修改模板 |
|---|---------|--------------|------|---------|
| 9 | `PagingWidget` 子控件 | objectName（中文不翻译） | 翻译后 objectName 可能变化 | 对 Edit_Page/Edit_Page_P 加 `setAccessibleName("Paging_CurrentPage")` |
| 10 | `EncryptionPage` 子控件 | objectName | 同上 | 对 ensureBtn/passwdEdit 加 `setAccessibleName("Encrypt_ConfirmBtn")` |
| 11 | `HandleMenu` 子控件 | objectName | 同上 | 对 defaultshape/handleshape 加 setAccessibleName |

### 无需修改（已有注册/显式名）

| 控件 | 名称来源 | 当前名称 |
|------|---------|---------|
| MainWindow | SET_FORM_ACCESSIBLE | MainWindow |
| Central | SET_FORM_ACCESSIBLE | Central |
| CentralDocPage | SET_FORM_ACCESSIBLE | CentralDocPage |
| CentralNavPage | SET_FORM_ACCESSIBLE | CentralNavPage |
| DocSheet | SET_FORM_ACCESSIBLE | DocSheet |
| DocTabBar | SET_FORM_ACCESSIBLE | DocTabBar |
| TitleWidget | SET_FORM_ACCESSIBLE | TitleWidget |
| SheetSidebar | SET_FORM_ACCESSIBLE | SheetSidebar |
| SheetBrowser | SET_FORM_ACCESSIBLE | SheetBrowser |
| ThumbnailWidget | SET_FORM_ACCESSIBLE | ThumbnailWidget |
| CatalogWidget | SET_FORM_ACCESSIBLE | CatalogWidget |
| BookMarkWidget | SET_FORM_ACCESSIBLE | BookMarkWidget |
| NotesWidget | SET_FORM_ACCESSIBLE | NotesWidget |
| SearchResWidget | SET_FORM_ACCESSIBLE | SearchResWidget |
| Menu_Browser | setAccessibleName | Menu_Browser |
| Menu_BookMark | setAccessibleName | Menu_BookMark |
| Menu_Note | setAccessibleName | Menu_Note |

## 实施优先级

### P0（影响核心自动化能力）
1. TitleMenu 菜单按钮 → get AT-SPI anchor for 主菜单操作
2. FindWidget 查找输入框 → get AT-SPI anchor for 搜索功能
3. 文档区域交互元素（BrowserPage/SheetBrowser 子控件）

### P1（影响功能覆盖）
4. ScaleWidget 缩放控件
5. SlidePlayWidget 幻灯片控件
6. SheetSidebar 侧边栏按钮

### P2（提升稳定性）
7. PagingWidget 翻页控件子元素
8. EncryptionPage 加密页面
9. HandleMenu 工具菜单

## 验证断言

实施后，以下 AT 用例应可通过元素名称定位：

```yaml
# 主菜单按钮
- action: element_action
  selector:
    name: "TitleMenu_MenuBtn"
  do: click

# 查找输入框  
- action: element_action
  selector:
    name: "FindWidget_SearchInput"
    role: "text"
  do: click
- action: keyboard_type
  text: "test"
```

## 动态风险说明

- `_N` 去重：DTK 动态创建的同名控件会在 AT-SPI 树中自动加 `_1`/`_2` 后缀
- 弹窗控件：DDialog/DFileDialog 仅在打开时出现在 AT-SPI 树中
- 菜单项：纯瞬态元素，只能用 `dtk_main_menu`/`dtk_context_menu`，不可用 `element_action`
- [INFERENCE] 部分控件需运行时验证实际 AT-SPI 名称