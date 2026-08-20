# deepin-reader AT-SPI 实施清单

> 基于源码静态推导。缺口仅在此处列出，其他文档引用条目号。

---

## 覆盖统计

| 指标 | 值 |
|------|-----|
| 已注册 AT-SPI 类数 | 14 个自定义类 + 16 个通用 Qt/DTK 类 |
| 显式 setAccessibleName 调用 | ~15 处 |
| 显式 setObjectName 调用 | ~20 处 |
| 源码 AT-SPI 覆盖率(控件级) | ~60% |
| 源码 AT-SPI 覆盖率(交互元素) | ~65% |

## 命名规则

所有 AT-SPI 名称遵守 `{RolePrefix}_{名称}` 格式，`_` 分隔符：
- `Form_` — QAccessible::Form 角色（容器/面板/窗口）
- `Button_` — QAccessible::Button 角色（按钮/可点击）
- `Label_` — QAccessible::StaticText 角色（文本/标签）
- `Editable_` — QAccessible::EditableText 角色（输入框）
- `Slider_` — QAccessible::Slider 角色（滑块/进度）

**优先级**：`accessibleName()` > `accessibleName via setAccessibleName()` > `objectName` > fallback（text/toolTip/classname）
**去重**：同名时追加 `_N`

---

## 缺口一：需补充 setAccessibleName

> 以下控件缺少显式 AT-SPI 名称锚点，依赖不可靠的文本 fallback 或泛化类名。

### GAP-001: 缩小按钮 (ScaleWidget → SP_DecreaseElement)

| 属性 | 值 |
|------|-----|
| **目标名称** | `Button_ZoomOut` |
| **文件:方法** | `reader/widgets/ScaleWidget.cpp` → `ScaleWidget::initWidget()` |
| **插入位置** | 第72行 `pPreBtn->setObjectName("SP_DecreaseElement");` 之后 |
| **修改模板** | `pPreBtn->setAccessibleName("Button_ZoomOut");` |
| **优先级** | 建议 |
| **验证断言** | AT 用例: `定位: accessible_id=Button_ZoomOut` |

### GAP-002: 放大按钮 (ScaleWidget → SP_IncreaseElement)

| 属性 | 值 |
|------|-----|
| **目标名称** | `Button_ZoomIn` |
| **文件:方法** | `reader/widgets/ScaleWidget.cpp` → `ScaleWidget::initWidget()` |
| **插入位置** | 第79行 `pNextBtn->setObjectName("SP_IncreaseElement");` 之后 |
| **修改模板** | `pNextBtn->setAccessibleName("Button_ZoomIn");` |
| **优先级** | 建议 |
| **验证断言** | AT 用例: `定位: accessible_id=Button_ZoomIn` |

### GAP-003: 缩放下拉箭头 (ScaleWidget → editArrowBtn)

| 属性 | 值 |
|------|-----|
| **目标名称** | `Button_ZoomArrow` |
| **文件:方法** | `reader/widgets/ScaleWidget.cpp` → `ScaleWidget::initWidget()` |
| **插入位置** | 第52行 `m_arrowBtn->setObjectName("editArrowBtn");` 之后 |
| **修改模板** | `m_arrowBtn->setAccessibleName("Button_ZoomArrow");` |
| **优先级** | 建议 |
| **验证断言** | AT 用例: `定位: accessible_id=Button_ZoomArrow` |

### GAP-004: 标签栏左滚按钮 (DocTabBar → leftButton)

| 属性 | 值 |
|------|-----|
| **目标名称** | `Button_TabScrollLeft` |
| **文件:方法** | `reader/uiframe/DocTabBar.cpp` → DocTabBar 构造函数(或DTabBar内部) |
| **插入位置** | DTabBar 内部 leftButton 对象创建处，或 CentralDocPage.cpp:120 after findChild |
| **修改模板** | `leftButton->setAccessibleName("Button_TabScrollLeft");` |
| **优先级** | 建议 |
| **验证断言** | AT 用例: `定位: accessible_id=Button_TabScrollLeft` |

### GAP-005: 标签栏右滚按钮 (DocTabBar → rightButton)

| 属性 | 值 |
|------|-----|
| **目标名称** | `Button_TabScrollRight` |
| **文件:方法** | 同上 |
| **插入位置** | 同上 |
| **修改模板** | `rightButton->setAccessibleName("Button_TabScrollRight");` |
| **优先级** | 建议 |
| **验证断言** | AT 用例: `定位: accessible_id=Button_TabScrollRight` |

### GAP-006: 标签栏添加标签按钮 (DocTabBar → AddButton)

| 属性 | 值 |
|------|-----|
| **目标名称** | `Button_TabAdd` |
| **文件:方法** | 同上 |
| **插入位置** | DTabBar 内部 AddButton 对象创建处，或 CentralDocPage.cpp:122 after findChild |
| **修改模板** | `addButton->setAccessibleName("Button_TabAdd");` |
| **优先级** | 建议 |
| **验证断言** | AT 用例: `定位: accessible_id=Button_TabAdd` |

### GAP-007: 搜索栏上一个按钮 (FindWidget → SP_ArrowUpBtn)

| 属性 | 值 |
|------|-----|
| **目标名称** | `Button_FindPrev` |
| **文件:方法** | `reader/widgets/FindWidget.cpp` → `FindWidget::initWidget()` |
| **插入位置** | 第130行 `setDisabled(true);` 之后，connect 之前 |
| **修改模板** | `m_findPrevButton->setAccessibleName("Button_FindPrev");` |
| **优先级** | 建议 |
| **验证断言** | AT 用例: `定位: accessible_id=Button_FindPrev` |

### GAP-008: 搜索栏下一个按钮 (FindWidget → SP_ArrowDownBtn)

| 属性 | 值 |
|------|-----|
| **目标名称** | `Button_FindNext` |
| **文件:方法** | `reader/widgets/FindWidget.cpp` → `FindWidget::initWidget()` |
| **插入位置** | 第137行 `setDisabled(true);` 之后，connect 之前 |
| **修改模板** | `m_findNextButton->setAccessibleName("Button_FindNext");` |
| **优先级** | 建议 |
| **验证断言** | AT 用例: `定位: accessible_id=Button_FindNext` |

### GAP-009: 搜索栏关闭按钮 (FindWidget → closeButton)

| 属性 | 值 |
|------|-----|
| **目标名称** | `Button_FindClose` |
| **文件:方法** | `reader/widgets/FindWidget.cpp` → `FindWidget::initWidget()` |
| **插入位置** | 第143行 `closeButton->setObjectName("closeButton");` 之后 |
| **修改模板** | `closeButton->setAccessibleName("Button_FindClose");` |
| **优先级** | 建议 |
| **验证断言** | AT 用例: `定位: accessible_id=Button_FindClose` |

### GAP-010: 无文档时导航页开始阅读按钮 (CentralNavPage)

| 属性 | 值 |
|------|-----|
| **目标名称** | `Button_StartReading` |
| **文件:方法** | `reader/uiframe/CentralNavPage.cpp` → 构造函数 |
| **插入位置** | 文件打开按钮创建处后，第23行附近 |
| **修改模板** | `openBtn->setAccessibleName("Button_StartReading");` (需确认控件变量名) |
| **优先级** | 建议 |
| **验证断言** | AT 用例: `定位: accessible_id=Button_StartReading` |

---

## 缺口二：需补充 setObjectName

> 以下控件既无 setAccessibleName 也无 setObjectName，fallback 完全依赖翻译文本（不稳定）。

### GAP-011: 文档区域右键菜单 (BrowserMenu)

| 属性 | 值 |
|------|-----|
| **目标名称** | `Menu_DocView` |
| **文件:方法** | `reader/browser/BrowserMenu.cpp` → 构造函数 |
| **插入位置** | 构造函数 `setAccessibleName("Menu_DocView");` 或 `setObjectName("DocViewMenu");` |
| **修改模板** | `this->setAccessibleName("Menu_DocView");` |
| **优先级** | 建议 |
| **验证断言** | AT 用例: `定位: accessible_id=Menu_DocView` |

### GAP-012: 缩放比例菜单 (ScaleMenu)

| 属性 | 值 |
|------|-----|
| **目标名称** | `Menu_Scale` |
| **文件:方法** | `reader/widgets/ScaleMenu.cpp` → 构造函数 |
| **插入位置** | 构造函数中 |
| **修改模板** | `this->setAccessibleName("Menu_Scale");` |
| **优先级** | 建议 |
| **验证断言** | AT 用例: `定位: accessible_id=Menu_Scale` |

---

## 缺口三：动态/复杂场景（需运行时确认）

> 以下元素在当前静态分析中无法完全确定 AT-SPI 名称，标为 `[INFERENCE]`。

| 项目 | 说明 | 运行时确认方法 |
|------|------|---------------|
| DTitlebar 按钮 AT 名 | `DTitlebarDWindowOptionButton` 等 | `accerciser` 或 `youqu at dump` 确认 |
| 文档标签文本 | DTabBar 内部 tab 标签 | `youqu at dump` 抓取 |
| 侧栏切换按钮 objName | `Button_Catalog` 等实际 objName | `youqu at dump` 抓取 |
| 对话框 objectName | 各 DDialog 子类 | `youqu at dump` 抓取 |
| 附加按钮 + 全屏按钮 | DTitlebarDWindowQuitFullscreenButton | `accerciser` 检测 |
| 菜单项渲染名 | QAction 文本在 DMenu 中 AT-SPI 暴露名 | `youqu at dump` 抓取 |

---

## 无需修改的控件

> 已有可靠 AT-SPI 名称，无需修改。

| 元素 | 来源 | 名称 |
|------|------|------|
| MainWindow | SET_FORM_ACCESSIBLE | `Form_MainWindow` |
| Central | SET_FORM_ACCESSIBLE | `Form_Central` |
| CentralDocPage | SET_FORM_ACCESSIBLE | `Form_CentralDocPage` |
| CentralNavPage | SET_FORM_ACCESSIBLE | `Form_CentralNavPage` |
| DocSheet | SET_FORM_ACCESSIBLE | `Form_DocSheet` |
| DocTabBar | SET_FORM_ACCESSIBLE | `Form_DocTabBar` |
| TitleWidget | SET_FORM_ACCESSIBLE | `Form_TitleWidget` |
| SheetSidebar | SET_FORM_ACCESSIBLE | `Form_SheetSidebar` |
| SheetBrowser | SET_FORM_ACCESSIBLE | `Form_SheetBrowser` |
| 5 个侧栏视图 | SET_FORM_ACCESSIBLE | `Form_CatalogWidget` 等 |
| 缩略图切换按钮 | setAccessibleName | `Button_ThumbnailToggle` |
| 标题栏主菜单 | setAccessibleName | `Menu_Title` |
| 工具子菜单 | setAccessibleName | `Menu_Hand` |
| 搜索输入框 | setAccessibleName | `Form_findSearchEdit_P` |
| 搜索框内部输入 | setAccessibleName | `DLineEditChildLineEdit` |
| 侧栏切换按钮 | setAccessibleName 动态 | `Button_{objName}` |
| 滚动条 | setAccessibleName | `verticalScrollBar` / `horizontalScrollBar` |
| Tips 提示 | setAccessibleName | `Tips` |

---

## 实施摘要

| 缺口类别 | 数量 | 严重度 |
|---------|------|--------|
| 必做（无任何 AT-SPI 锚点） | 2 (GAP-011, GAP-012) | 必做 |
| 建议（fallback 易碎，依赖文本/翻译） | 8 (GAP-001~009 中部分) | 建议 |
| [INFERENCE] 需运行时确认 | 7 | 运行时 |

> **优先实施**: GAP-011 (BrowserMenu setAccessibleName) 和 GAP-012 (ScaleMenu setAccessibleName)
> 这 2 个是完整菜单缺失可访问名称的关键缺口。其余 GAP-001 到 GAP-010 的按钮/控件已通过 objectName 有一定 fallback，但不稳定。