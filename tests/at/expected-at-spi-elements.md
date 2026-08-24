# 预期 AT-SPI 元素清单 — deepin-reader

> 推导：本地源码 + 运行时 `at-tree.yaml` 对账。名称 = 角色前缀 + (显式 accessibleName | 注册 fallback | text/objectName)。
> `_N` 为多实例运行期去重，属动态值。已与 at-tree.yaml 中实际观测名对齐确认。

## 命名规则
`名称 = <Role前缀>_<已显式/回退名>`；Role 前缀：Form/Button/Label/Editable/Slider（accessibledefine.h:33-55）。多实例自动 `_1/_2…`。

| 预期名 | Role | 触发 | 使能/可见条件 | 推导链 |
|---|---|---|---|---|
| Form_DMainWindow | Form | 启动 | 始终 | 注册表 accessible.h:49 |
| Form_DTitlebar | Form | 启动 | 始终 | DTitlebar objectName 回退 |
| Form_TitleWidget | Form | 启动 | 始终 | 注册表 accessible.h:55 |
| Button_ThumbnailToggle | Button | 左键 | 有文档时 | TitleWidget.cpp:22 setAccessibleName |
| Form_scaleEdit_P | Form | 缩放框 | 有文档时 | ScaleWidget.cpp:42 objectName |
| DLineEditChildLineEdit | Editable | 缩放输入 | 同上 | ScaleWidget.cpp:43 lineEdit |
| Form_Central | Form | 启动 | 始终 | 注册表 accessible.h:50 |
| Form_CentralNavPage | Form | 空白页 | 无文档 | 注册表 accessible.h:52 |
| Button_SelectFile | Button | 点击 | 空状态 | CentralNavPage.cpp:44 |
| Label_Icon | Label | 展示 | 空状态 | CentralNavPage.cpp:58 |
| Form_CentralDocPage / Form_DocSheet | Form | 打开文档 | 有文档 | 注册表 accessible.h:53/51 |
| Form_DocTabBar | Form | 多标签 | 多文档 | 注册表 accessible.h:54 |
| DTitlebarDWindowClose/Min/Max/QuitFullscreenButton | Button | 标题栏 | 始终 | DTK titlebar 默认 |
| Form_SheetSidebar | Form | 打开文档 | 有文档 | 注册表 accessible.h:56 |
| Button_thumbnail | Button | 侧边栏 | 有文档 | SheetSidebar.cpp:363 |
| Button_catalog | Button | 侧边栏 | 有文档 | SheetSidebar.cpp:363 |
| Button_bookmark | Button | 侧边栏 | 有文档 | SheetSidebar.cpp:363 |
| Button_annotation | Button | 侧边栏 | 有文档 | SheetSidebar.cpp:363 |
| Button_search | Button | 侧边栏 | 有文档 | SheetSidebar.cpp:363 |
| Form_ThumbnailWidget | Form | 缩略图tab | — | 注册表 accessible.h:58 |
| View_ImageList | 视图 | 缩略图列表 | 有文档 | ThumbnailWidget.cpp:38 |
| Form_Paging | Form | 缩略图分页 | 有文档 | ThumbnailWidget.cpp:43 |
| Button_ThumbnailPre / Button_ThumbnailNext | Button | 翻页 | >1页 | PagingWidget.cpp:100/107 |
| pageEdit | Editable | 跳页输入 | 有文档 | PagingWidget.cpp:84 |
| CurrentPage / Label_TotalPage | Label | 展示 | 有文档 | PagingWidget.cpp:74/112 |
| Form_CatalogWidget | Form | 目录tab | 有目录 | 注册表.h:59 |
| View_CatalogTree | 视图 | 目录树 | 有目录 | CatalogWidget.cpp:55 |
| Form_BookMarkWidget | Form | 书签tab | — | 注册表.h:60 |
| Button_BookmarkAdd | Button | 添加书签 | 有文档 | BookMarkWidget.cpp:52 |
| Form_NotesWidget | Form | 批注tab | — | 注册表.h:61 |
| Button_NotesAdd | Button | 添加批注 | 有文档 | NotesWidget.cpp:55 |
| Form_SearchResWidget | Form | 搜索结果tab | 搜索后 | 注册表.h:62 |
| Menu_Hand | Menu | 主菜单工具栏 | 有文档 | TitleMenu.cpp:43 |
| DTitlebarThemeMenu | Menu | 主题 | 始终 | DTK titlebar |

## 由翻译文本做 fallback（易碎，需关注）
主菜单动作 New window/New tab/Save/Save as/Display in file manager/Magnifer/Search/Print（TitleMenu.cpp:27-39）：仅 setObjectName，无 setAccessibleName，AT 名依赖 objectName 回退，翻译切换时名称不变（objectName 稳定），但显式 setAccessibleName 更稳。
