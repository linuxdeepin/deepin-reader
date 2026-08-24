# AT-SPI 实施清单 — deepin-reader

> 缺口号 G1..Gn 唯一引用点。验证=本地编译通过（构建命令退出码 0 + 产物生成）。新增一律用 `setAccessibleName`（直接作用于 AT-SPI Name），英文稳定不含翻译文本。

## 覆盖统计（静态）
- 已注册 + 工厂类：16 应用级 + Qt 4 类 + DTK 15 类（accessible.h:49-135）。
- 显式 setAccessibleName 调用点：**35 处**（repo 内）。
- setObjectName 调用点：约 40 处（大部分作为回退锚点）。
- 与运行时 at-tree.yaml（357 个 name 节点）对账：主路径 95%+ 已可定位。

## 缺口明细

### G1 — DListView/DTreeView 无工厂注册（建议）
- 位置：`reader/app/accessible.h:128-133`（USE_ACCESSIBLE 列表）。
- 现状：`DListView`/`DTreeView`/`DTextEdit`/`DProgressBar`/`DComboBox` 已 include 但 **未** SET_*_ACCESSIBLE / USE_ACCESSIBLE，走 Qt 默认 accessible。
- 影响：通过 setAccessibleName 显式名仍生效（View_ImageList/View_CatalogTree 运行期 OK），但无 ActionInterface/自定义 role。
- 修改模板（追加到 accessible.h USE_ACCESSIBLE 区）：
  ```cpp
  SET_FORM_ACCESSIBLE(DListView, m_w->objectName().isEmpty() ? "DListView" : m_w->objectName())
  SET_FORM_ACCESSIBLE(DTreeView, m_w->objectName().isEmpty() ? "DTreeView" : m_w->objectName())
  // accessibleFactory() 中：
  USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DListView);
  USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DTreeView);
  ```
- 验证断言：dump 中 `Form_View_ImageList`（原 View_ImageList 带上 Form_ 前缀）可被 AT 命中。

### G2 — 主菜单 QAction 无显式 AT 名（建议）
- 位置：`reader/uiframe/TitleMenu.cpp:27-39`（createAction:143-152）。
- 现状：仅 `setObjectName("New window"/...)`（TitleMenu.cpp:148），AT 名 fallback=objectName，改翻译不改名，但未显式。
- 修改模板（createAction 内，new 之后）：
  ```cpp
  action->setAccessibleName("MenuAction_" + objName);
  ```
  即 `MenuAction_New window / MenuAction_Save / MenuAction_Search / MenuAction_Print` 等。
- 验证断言：menu 展开后 AT 可定位 `MenuAction_Save`；文档未改动时 Save 为 disabled。

### G3 — FindWidget 上下翻按钮仅 objectName（建议）
- 位置：`reader/widgets/FindWidget.cpp:129-143`（SP_ArrowUpBtn / SP_ArrowDownBtn / closeButton）。
- 现状：DToolButton fallback=objectName → `Button_SP_ArrowUpBtn` 已稳定，但 semantic 名不友好。
- 模板：`m_findPrevButton->setAccessibleName("Button_FindPrev"); m_findNextButton->setAccessibleName("Button_FindNext");`
- 验证断言：查找框展开后 AT 可定位 Button_FindPrev / Button_FindNext。

### G4 — EncryptionPage 密码框（建议）
- 位置：`reader/widgets/EncryptionPage.cpp:43-50`。
- 现状：passwdEdit / ensureBtn 仅 objectName；DPasswordEdit 无工厂注册。
- 模板：`edit->setAccessibleName("Editable_Password"); m_nextbutton->setAccessibleName("Button_Ensure");`
- 验证断言：加密保存对话框内 AT 可定位 Editable_Password。

### G5 — SideBarImageListview 右键菜单条目（建议）
- 位置：`reader/sidebar/SideBarImageListview.cpp:275-311`。
- 现状：菜单容器有 Menu_Note/Menu_BookMark/Menu_Catalog，条目只 addAction(tr(...))。
- 模板：`copyAction->setAccessibleName("MenuAction_Copy"); delAction->setAccessibleName("MenuAction_RemoveAnnotation"); …`
- 验证断言：缩略图右键后 AT 命中 MenuAction_Copy。

## 无需修改（静态确认，与运行时一致）
Form_DMainWindow / Button_SelectFile / Button_ThumbnailToggle / Button_thumbnail..search / View_ImageList / View_CatalogTree / Button_BookmarkAdd / Button_NotesAdd / pageEdit / Button_ThumbnailPre/Next / Menu_Hand 等均已有 setAccessibleName 或注册回退，实测名称一致（at-tree.yaml 对账通过）。

## 动态风险
- 同对象多实例（多窗口/多 tab）出现 `_1/_2` 后缀，为运行期去重（accessibledefine.h:61-71），非静态可预测。
- Form_scaleEdit_P 同窗口存在 _1 实例（at-tree.yaml 实测），用例定位时按"名+上下文"组合。
- Label_Drag documents here 含翻译文本（CentralNavPage.cpp:23），语言切换变名，优先用 Button_SelectFile 定位。

## 实施顺序
① 编译前提检查（QWidget 派生、模板在 new 后、不动声明）→ ② 每条目本地编译 `cmake --build build` 退出码 0 + 无新增 warning → ③ 冒烟 → ④ 运行期 dump 对账 → ⑤ 静态计数勾对（setAccessibleName 数 35 → 35+新条目）。
