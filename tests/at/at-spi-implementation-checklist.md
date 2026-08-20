# Deepin Reader AT-SPI 实施清单

> 生成时间: 2026-08-20
> 推导方式: 源码静态分析 via remote-codebase MCP

## 命名约定

deepin-reader 使用 `getAccessibleName()` 函数生成 AT-SPI 名称，规则为：
```
<RolePrefix>_<accessibleName|objectName|fallback>
```

RolePrefix 映射：
- `QAccessible::Form` → `Form`
- `QAccessible::Button` → `Button`
- `QAccessible::StaticText` → `Label`
- `QAccessible::EditableText` → `Editable`
- `QAccessible::Slider` → `Slider`

已注册的 custom accessible 类：14 个 `SET_FORM_ACCESSIBLE` + 15 个 Qt/DTK 通用宏。

**优先级**：`setAccessibleName()` > `objectName()` > 宏 fallback > 类型名 fallback

## 显式 setAccessibleName/objectName 对账

| 文件 | 调用 | 名称 |
|------|------|------|
| reader/uiframe/TitleWidget.cpp | setAccessibleName | Button_ThumbnailToggle |
| reader/uiframe/CentralNavPage.cpp | setAccessibleName | Button_SelectFile |
| reader/uiframe/CentralNavPage.cpp | setAccessibleName | Label_Icon |

## 覆盖统计

| 分类 | 数量 | 备注 |
|------|------|------|
| 已注册自定义控件 | 14 | 通过 SET_FORM_ACCESSIBLE 注册, 名称 = Form_<ClassName> |
| 有显式 setAccessibleName | 3 | 直接设置 AT-SPI 名称 |
| 有显式 setObjectName | 0 | 无显式 setObjectName 调用 |
| 泛型 fallback 命名 | ~25 | DTK/Qt 通用宏（DTitlebar, DLabel 等） |
| 缺口（需补 setAccessibleName） | 14 | 子组件无任何 AT 锚点 |
| 总计 | ~56 | 含容器、可交互控件、菜单项 |

## 缺口明细

### G01: ScaleWidget 整体容器

- **目标名称**: `Form_ScaleWidget`
- **文件**: `reader/widgets/ScaleWidget.cpp`
- **插入位置**: `ScaleWidget 构造函数`，在布局设置完成后
- **修改模板**:
  ```cpp
  // 在构造函数末尾添加
  setAccessibleName("ScaleWidget");
  ```
- **验证断言**: `assert_element` selector: `{name: "Form_ScaleWidget", role: "form"}`

### G02: ScaleWidget 缩放按钮

- **目标名称**: 缩放比例-50% / 缩放比例+50% / 缩放比例100% 等
- **文件**: `reader/widgets/ScaleWidget.cpp`
- **插入位置**: 创建每个 QPushButton 后，如 `m_pBtnReduce` / `m_pBtnEnlarge` / `m_pBtnRestore`
- **修改模板**:
  ```cpp
  m_pBtnReduce->setAccessibleName("ScaleWidget_ReduceBtn");
  m_pBtnEnlarge->setAccessibleName("ScaleWidget_EnlargeBtn");
  m_pBtnRestore->setAccessibleName("ScaleWidget_RestoreBtn");
  ```
- **验证断言**: `assert_element` selector: `{name: "Button_ScaleWidget_ReduceBtn", role: "push button"}`

### G03: SlidePlayWidget 整体容器

- **目标名称**: `Form_SlidePlayWidget`
- **文件**: `reader/widgets/SlidePlayWidget.cpp`
- **插入位置**: 构造函数末尾
- **修改模板**:
  ```cpp
  setAccessibleName("SlidePlayWidget");
  ```
- **验证断言**: `assert_element` selector: `{name: "Form_SlidePlayWidget", role: "form"}`

### G04: SlidePlayWidget 控制按钮

- **目标名称**: 播放控制按钮
- **文件**: `reader/widgets/SlidePlayWidget.cpp`
- **插入位置**: 创建按钮后，对每个播放控制按钮
- **修改模板**:
  ```cpp
  m_playBtn->setAccessibleName("SlidePlay_PlayBtn");
  m_prevBtn->setAccessibleName("SlidePlay_PrevBtn");
  m_nextBtn->setAccessibleName("SlidePlay_NextBtn");
  ```
- **验证断言**: `assert_element` selector: `{name: "Button_SlidePlay_PlayBtn", role: "push button"}`

### G05: SlideWidget 整体容器

- **目标名称**: `Form_SlideWidget`
- **文件**: `reader/widgets/SlideWidget.cpp`
- **插入位置**: 构造函数末尾
- **修改模板**:
  ```cpp
  setAccessibleName("SlideWidget");
  ```
- **验证断言**: `assert_element` selector: `{name: "Form_SlideWidget", role: "form"}`

### G06: EncryptionPage 整体容器

- **目标名称**: `Form_EncryptionPage`
- **文件**: `reader/widgets/EncryptionPage.cpp`
- **插入位置**: 构造函数末尾
- **修改模板**:
  ```cpp
  setAccessibleName("EncryptionPage");
  ```
- **验证断言**: `assert_element` selector: `{name: "Form_EncryptionPage", role: "form"}`

### G07: EncryptionPage 按钮

- **目标名称**: 加密页面按钮
- **文件**: `reader/widgets/EncryptionPage.cpp`
- **插入位置**: 创建按钮后
- **修改模板**:
  ```cpp
  m_confirmBtn->setAccessibleName("Encryption_ConfirmBtn");
  m_cancelBtn->setAccessibleName("Encryption_CancelBtn");
  ```
- **验证断言**: `assert_element` selector: `{name: "Button_Encryption_ConfirmBtn", role: "push button"}`

### G08: HandleMenu 整体容器

- **目标名称**: `Form_HandleMenu`
- **文件**: `reader/widgets/HandleMenu.cpp`
- **插入位置**: 构造函数末尾
- **修改模板**:
  ```cpp
  setAccessibleName("HandleMenu");
  ```
- **验证断言**: `assert_element` selector: `{name: "Form_HandleMenu", role: "form"}`

### G09: TextEditShadowWidget 整体容器

- **目标名称**: `Form_TextEditShadowWidget`
- **文件**: `reader/widgets/TextEditWidget.cpp`
- **插入位置**: TextEditShadowWidget 构造函数末尾
- **修改模板**:
  ```cpp
  setAccessibleName("TextEditShadowWidget");
  ```
- **验证断言**: `assert_element` selector: `{name: "Form_TextEditShadowWidget", role: "form"}`

### G10: RestoreTipWidget 整体容器

- **目标名称**: `Form_RestoreTipWidget`
- **文件**: `reader/widgets/RestoreTipWidget.cpp`
- **插入位置**: 构造函数末尾
- **修改模板**:
  ```cpp
  setAccessibleName("RestoreTipWidget");
  ```
- **验证断言**: `assert_element` selector: `{name: "Form_RestoreTipWidget", role: "form"}`

### G11: ProgressDialog 整体容器

- **目标名称**: `Form_ProgressDialog`
- **文件**: `reader/widgets/ProgressDialog.cpp`
- **插入位置**: 构造函数末尾
- **修改模板**:
  ```cpp
  setAccessibleName("ProgressDialog");
  ```
- **验证断言**: `assert_element` selector: `{name: "Form_ProgressDialog", role: "dialog"}`

### G12: ProgressDialog 按钮

- **目标名称**: 进度对话框按钮
- **文件**: `reader/widgets/ProgressDialog.cpp`
- **插入位置**: 创建按钮后
- **修改模板**:
  ```cpp
  m_cancelBtn->setAccessibleName("Progress_CancelBtn");
  ```
- **验证断言**: `assert_element` selector: `{name: "Button_Progress_CancelBtn", role: "push button"}`

### G13: SecurityDialog 整体容器

- **目标名称**: `Form_SecurityDialog`
- **文件**: `reader/widgets/SecurityDialog.cpp`
- **插入位置**: 构造函数末尾
- **修改模板**:
  ```cpp
  setAccessibleName("SecurityDialog");
  ```
- **验证断言**: `assert_element` selector: `{name: "Form_SecurityDialog", role: "dialog"}`

### G14: SecurityDialog 按钮

- **目标名称**: 安全对话框按钮
- **文件**: `reader/widgets/SecurityDialog.cpp`
- **插入位置**: 创建按钮后
- **修改模板**:
  ```cpp
  m_confirmBtn->setAccessibleName("Security_ConfirmBtn");
  m_cancelBtn->setAccessibleName("Security_CancelBtn");
  ```
- **验证断言**: `assert_element` selector: `{name: "Button_Security_ConfirmBtn", role: "push button"}`

## 无需修改的控件

| 名称 | 原因 |
|------|------|
| 所有 14 个 SET_FORM_ACCESSIBLE 类 | 已有 AT 注册，名称 = Form_<ClassName> |
| Button_ThumbnailToggle | 显式 setAccessibleName |
| Button_SelectFile | 显式 setAccessibleName |
| Label_Icon | 显式 setAccessibleName |
| DTitlebar 系列按钮 | DTK fallback 自动命名 |
| 菜单项 | 使用文本标记，非 AT 名缺口 |
| BrowserMenu | 有 SET_FORM_ACCESSIBLE |
| DSearchEdit | 有 DTK 通用宏注册 |
| FindWidget | 有 SET_FORM_ACCESSIBLE(FindWidget) |

## 动态命名风险

- 同名控件去重：`getAccessibleName()` 中 `accessibleMap` 机制对有重复名称的控件自动添加 `_N` 后缀。
- 翻译文本依赖：DTK 按钮使用 `text()` 作为 fallback 名称，受语言环境影响。
- 空名称风险：未设 setAccessibleName/objectName 的控件使用类型名 fallback（如 `widget`、`qpushbutton`），多个实例会产生 `_1`、`_2` 等动态编号。

## 优先级建议

| 优先级 | 缺口 | 理由 |
|--------|------|------|
| P0（立即） | G02, G04, G07, G12, G14 | 交互按钮，直接影响 AT 测试定位 |
| P1（高） | G01, G03, G05, G06, G08, G09, G10 | 容器但包含交互控件，影响测试可靠性 |
| P2（中） | G11, G13 | 对话框整体，较少被 AT 定位 |