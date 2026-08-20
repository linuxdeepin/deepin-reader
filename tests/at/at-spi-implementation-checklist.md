# AT-SPI Implementation Checklist — Deepin Reader

> Static gap analysis via remote-codebase MCP
> Naming convention: `Reader.<WidgetType>`

---

## Gap Statistics

| Metric | Value |
|--------|-------|
| Total app-specific widget classes | 14 (registered in accessible.h) |
| Total interactive elements analyzed | 45+ |
| ✅ Already with setAccessibleName | ~30 |
| ⚠️ objectName fallback only (brittle) | ~10 |
| 🔴 No AT anchor at all (high priority) | 5 |
| 🔴 No AT anchor (medium priority) | 2 |

---

## ⚠️ 仅 objectName fallback 元素（建议改进）

这些元素依赖 `SET_FORM_ACCESSIBLE(DWidget/DLabel, m_w->objectName())` 或 `SET_BUTTON_ACCESSIBLE(DIconButton, m_w->objectName())` 生成 AT-SPI 名，objectName 是翻译非友好名，建议增加直接 setAccessibleName。

| # | 位置（文件:行号） | 当前 AT 名 (fallback) | 建议 | 已验证编译 |
|---|-------------------|----------------------|------|-----------|
| G1 | reader/widgets/ScaleWidget.cpp:42 | DLineEdit "scaleEdit_P" | `scaleEdit->setAccessibleName("ScaleInput")` | — |
| G2 | reader/widgets/ScaleWidget.cpp:52 | DIconButton "editArrowBtn" | `m_arrowBtn->setAccessibleName("Button_ScaleMenuToggle")` | — |
| G3 | reader/widgets/ScaleWidget.cpp:72 | DIconButton objectName match | `pPreBtn->setAccessibleName("Button_ZoomOut")` | — |
| G4 | reader/widgets/ScaleWidget.cpp:79 | DIconButton objectName match | `pNextBtn->setAccessibleName("Button_ZoomIn")` | — |
| G5 | reader/widgets/FindWidget.cpp:129 | DIconButton "SP_ArrowUpBtn" | `m_findPrevButton->setAccessibleName("Button_FindPrev")` | — |
| G6 | reader/widgets/FindWidget.cpp:136 | DIconButton "SP_ArrowDownBtn" | `m_findNextButton->setAccessibleName("Button_FindNext")` | — |
| G7 | reader/widgets/FindWidget.cpp:143 | DDialogCloseButton "closeButton" | `closeButton->setAccessibleName("Button_FindClose")` | — |
| G8 | reader/widgets/EncryptionPage.cpp:46 | QLineEdit "passwdEdit" | `edit->setAccessibleName("PasswordInput")` | — |
| G9 | reader/widgets/EncryptionPage.cpp:50 | DPushButton "ensureBtn" | `m_nextbutton->setAccessibleName("Button_PasswordConfirm")` | — |
| G10 | reader/eyeprotection/EyeProtectionAction.cpp:64 | RoundColorWidget "eye_0~3" | 每组 `btn->setAccessibleName("ColorTheme_N")` | — |

---

## 🔴 无 AT 锚点（高优先级）

这些类既无 class-level AT 注册，也无 setAccessibleName/setObjectName 调用，AT-SPI 名会退化为 Qt 内部生成的不可靠名称。

| # | 文件:行号范围 | 类名 | 问题 | 修改模板 |
|---|---------------|------|------|----------|
| H1 | reader/widgets/SlidePlayWidget.cpp:156-166 | SlidePlayWidget | 按钮createBtn只设objectName | 在createBtn内增加 `btn->setAccessibleName("Button_Slide" + strname)` |
| H2 | reader/widgets/HandleMenu.cpp:19-41 | HandleMenu | m_textAction, m_handAction 只有objectName | 在initActions中新增 `m_textAction->setAccessibleName("Action_SelectText")`; `m_handAction->setAccessibleName("Action_HandTool")` |
| H3 | reader/browser/BrowserMenu.cpp:223-232 | BrowserMenu::createAction | action只设objectName | 增加 `action->setAccessibleName("Action_" + objectname)` |
| H4 | reader/uiframe/TitleMenu.cpp:144-153 | TitleMenu::createAction | action只设objectName | 增加 `action->setAccessibleName("Action_" + objName)` |
| H5 | reader/widgets/ScaleMenu.cpp:113-122 | ScaleMenu::createAction | action只设objectName | 增加 `action->setAccessibleName("Action_" + objName)` |

---

## ✅ 无需修改（已有类级注册或显式名）

| 类名 | 文件 | 已有机制 |
|------|------|----------|
| MainWindow | reader/MainWindow.cpp | ✅ SET_FORM_ACCESSIBLE + Menu_Title |
| Central | reader/uiframe/Central.cpp | ✅ SET_FORM_ACCESSIBLE |
| CentralDocPage | reader/uiframe/CentralDocPage.cpp | ✅ SET_FORM_ACCESSIBLE |
| CentralNavPage | reader/uiframe/CentralNavPage.cpp | ✅ SET_FORM_ACCESSIBLE + 4个显式名 |
| DocSheet | reader/uiframe/DocSheet.cpp | ✅ SET_FORM_ACCESSIBLE |
| DocTabBar | reader/uiframe/DocTabBar.cpp | ✅ SET_FORM_ACCESSIBLE |
| TitleWidget | reader/uiframe/TitleWidget.cpp | ✅ SET_FORM_ACCESSIBLE + Button_ThumbnailToggle |
| SheetSidebar | reader/sidebar/SheetSidebar.cpp | ✅ SET_FORM_ACCESSIBLE + createBtn动态名 |
| SheetBrowser | reader/browser/SheetBrowser.cpp | ✅ SET_FORM_ACCESSIBLE + Tips/scrollBars |
| BrowserMenu | reader/browser/BrowserMenu.cpp | ✅ Menu_Browser |
| CatalogWidget | reader/sidebar/CatalogWidget.cpp | ✅ SET_FORM_ACCESSIBLE + 2个显式名 |
| BookMarkWidget | reader/sidebar/BookMarkWidget.cpp | ✅ SET_FORM_ACCESSIBLE + 3个显式名 |
| NotesWidget | reader/sidebar/NotesWidget.cpp | ✅ SET_FORM_ACCESSIBLE + 3个显式名 |
| SearchResWidget | reader/sidebar/SearchResWidget.cpp | ✅ SET_FORM_ACCESSIBLE + View_ImageList |
| ThumbnailWidget | reader/sidebar/ThumbnailWidget.cpp | ✅ SET_FORM_ACCESSIBLE + 3个显式名 |
| PagingWidget | reader/widgets/PagingWidget.cpp | ✅ 6个显式setAccessibleName |
| FindWidget | reader/widgets/FindWidget.cpp | ✅ Form_findSearchEdit + DLineEditChildLineEdit (部分) |

---

## 动态风险提示

| 风险 | 说明 |
|------|------|
| 翻译文本作 AT 名 | `CentralNavPage` 中 `Label_Drag documents here` 和 `Label_format supported...` 使用翻译文本作为 accessibleName，不同语言下 AT-SPI 名称不一致 |
| 同名消歧 `_N` | 侧栏 4 个 Widget 共享 `View_ImageList` 名，运行时可能出现 `View_ImageList` / `View_ImageList_1` / ... 需验证 |
| createBtn 动态名 | `Button_Catalog` / `Button_BookMark` / `Button_Note` / `Button_Thumbnail` / `Button_search` 通过拼接生成，需确认每个 objName 唯一 |
| 菜单项无 AT 名 | 所有菜单项（TitleMenu/BrowserMenu/ScaleMenu/HandleMenu）的 action 只有 objectName，无 accessibleName → 菜单项可通过 `<role>` + `<text>` 定位 |