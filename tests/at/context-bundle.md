# Context Bundle — deepin-reader (AT-SPI)

Source: `at-tree-annotated.yaml`, `tests/at/docs/`.
Menu items under popup menus are referenced via `dtk_main_menu` / `dtk_context_menu`.

## 表1：元素-功能对照表

| 元素名 | 节点id | Role | 功能描述 | 可见条件 |
| --- | --- | --- | --- | --- |
| DTitlebarIconLabel | n6 | button | 应用图标/启动器按钮（左侧区域） | 文档打开后，标题栏左侧 |
| Button_ThumbnailToggle | n8 | check box | 标题栏缩略图视图切换开关 | 文档打开后，标题栏左侧 |
| DLineEditChildLineEdit | n11 | text | 缩放比例输入框（可输入/下拉缩放比例） | 文档打开后，标题栏左侧 |
| DTitlebarDWindowOptionButton | n18 | button | 主菜单按钮（打开 DTitlebarMainMenu） | 文档打开后，标题栏右侧 |
| DTitlebarDWindowMinButton | n19 | button | 最小化窗口 | 文档打开后，标题栏右侧 |
| DTitlebarDWindowMaxButton | n20 | button | 最大化/还原窗口 | 文档打开后，标题栏右侧 |
| DTitlebarDWindowQuitFullscreenButton | n21 | button | 退出全屏 / 全屏切换 | 文档打开后，标题栏右侧 |
| DTitlebarDWindowCloseButton | n22 | button | 关闭窗口 | 文档打开后，标题栏右侧 |
| Button_SelectFile | n28 | button | 初始界面“选择文件”打开文档 | 未打开文档（CentralNavPage）时 |
| Label_Icon | n29 | image | 初始界面文档图标 | 未打开文档（CentralNavPage）时 |
| 多数字员工并行工作站.pdf | n32 | page tab list | 标签页列表容器 | 已打开文档 |
| 多数字员工并行工作站.pdf | n33 | page tab | 当前文档标签（点击切换/激活） | 已打开文档 |
| 向左滚动 | n34 | button | 标签栏向左滚动 | 标签页过多时 |
| 向右滚动 | n35 | button | 标签栏向右滚动 | 标签页过多时 |
| DTabBarAddButton | n36 | button | 新增标签页 | 标签页≥2 时显示 |
| View_ImageList | n43 | list | 搜索结果列表 | 搜索面板展开 |
| View_ImageList | n45 | list | 注释列表 | 注释面板展开 |
| View_ImageList | n49 | list | 书签列表 | 书签面板展开 |
| Button_NotesAdd | n46 | button | 添加注释 | 注释面板展开 |
| Button_BookmarkAdd | n50 | button | 添加书签 | 书签面板展开 |
| View_CatalogTree | n54 | tree | 文档目录树（章节导航） | 目录面板展开 |
| pageEdit | n63 | text | 页码输入框（输入页码后 Enter 跳转） | 缩略图分页区 |
| Button_ThumbnailPre | n65 | button | 上一页 | 缩略图分页区 |
| Button_ThumbnailNext | n66 | button | 下一页 | 缩略图分页区 |
| Button_thumbnail | n68 | check box | 左侧栏“缩略图”切换按钮 | 左侧工具栏展开 |
| Button_catalog | n69 | check box | 左侧栏“目录”切换按钮 | 左侧工具栏展开 |
| Button_bookmark | n70 | check box | 左侧栏“书签”切换按钮 | 左侧工具栏展开 |
| Button_annotation | n71 | check box | 左侧栏“注释”切换按钮 | 左侧工具栏展开 |
| Button_search | n72 | check box | 左侧栏“搜索”切换按钮 | 左侧工具栏展开 |
| 新窗口 | n79 | menu item | 主菜单：新建窗口 | 打开 DTitlebarMainMenu |
| 新标签页 | n80 | menu item | 主菜单：新建标签页 | 打开 DTitlebarMainMenu |
| 保存 | n81 | menu item | 主菜单：保存当前文档 | 打开 DTitlebarMainMenu |
| 另存为 | n82 | menu item | 主菜单：另存为 | 打开 DTitlebarMainMenu |
| 在文件管理器中显示 | n83 | menu item | 主菜单：定位文件存储位置 | 打开 DTitlebarMainMenu |
| 放大镜 | n84 | menu item | 主菜单：启用放大镜 | 打开 DTitlebarMainMenu |
| 工具 | n85 | menu item | 主菜单：工具子菜单入口 | 打开 DTitlebarMainMenu |
| 选择工具 | n87 | menu item | 子菜单：文本选择工具 | 打开 工具 → Menu_Hand |
| 手形工具 | n88 | menu item | 子菜单：文档移动手形工具 | 打开 工具 → Menu_Hand |
| 搜索 | n89 | menu item | 主菜单：打开搜索窗口 | 打开 DTitlebarMainMenu |
| 打印 | n90 | menu item | 主菜单：打开打印/打印预览 | 打开 DTitlebarMainMenu |
| 主题 | n91 | menu item | 主菜单：主题子菜单入口 | 打开 DTitlebarMainMenu |
| 浅色 | n93 | menu item | 子菜单：浅色主题 | 打开 主题 → DTitlebarThemeMenu |
| 深色 | n94 | menu item | 子菜单：深色主题 | 打开 主题 → DTitlebarThemeMenu |
| 跟随系统 | n95 | menu item | 子菜单：跟随系统主题 | 打开 主题 → DTitlebarThemeMenu |
| 帮助 | n96 | menu item | 主菜单：查看帮助手册 | 打开 DTitlebarMainMenu |
| 关于 | n97 | menu item | 主菜单：查看版本与介绍 | 打开 DTitlebarMainMenu |
| 退出 | n98 | menu item | 主菜单：退出文档查看器 | 打开 DTitlebarMainMenu |
| 选择工具 | n100 | menu item | 右键菜单/手形工具菜单：文本选择工具 | 打开 Menu_Hand |
| 手形工具 | n101 | menu item | 右键菜单/手形工具菜单：文档移动工具 | 打开 Menu_Hand |
| DTitlebarIconLabel | n113 | button | 第二窗口实例：应用图标/启动器按钮 | 第二窗口打开后 |
| Button_ThumbnailToggle | n115 | check box | 第二窗口实例：标题栏缩略图切换 | 第二窗口打开后 |
| DLineEditChildLineEdit | n118 | text | 第二窗口实例：缩放比例输入框 | 第二窗口打开后 |
| DTitlebarDWindowOptionButton | n125 | button | 第二窗口实例：主菜单按钮 | 第二窗口打开后 |
| DTitlebarDWindowMinButton | n126 | button | 第二窗口实例：最小化窗口 | 第二窗口打开后 |
| DTitlebarDWindowMaxButton | n127 | button | 第二窗口实例：最大化/还原窗口 | 第二窗口打开后 |
| DTitlebarDWindowQuitFullscreenButton | n128 | button | 第二窗口实例：退出全屏 | 第二窗口打开后 |
| DTitlebarDWindowCloseButton | n129 | button | 第二窗口实例：关闭窗口 | 第二窗口打开后 |
| Button_SelectFile | n143 | button | 第二窗口实例：初始界面选择文件 | 第二窗口未打开文档时 |

## 表2：界面-元素映射

| 界面 | 包含元素 |
| --- | --- |
| 标题栏左区域 (Form_DTitlebarLeftArea) | DTitlebarIconLabel、Button_ThumbnailToggle、DLineEditChildLineEdit |
| 标题栏右区域 (Form_DTitlebarButtonArea) | DTitlebarDWindowOptionButton、DTitlebarDWindowMinButton、DTitlebarDWindowMaxButton、DTitlebarDWindowQuitFullscreenButton、DTitlebarDWindowCloseButton |
| 标题栏缩放控件 (Form_scaleEdit_P) | Button_ThumbnailToggle、DLineEditChildLineEdit |
| 左侧栏切换按钮组 (Form_SheetSidebar) | Button_thumbnail、Button_catalog、Button_bookmark、Button_annotation、Button_search |
| 搜索面板 (Form_SearchResWidget) | View_ImageList |
| 注释面板 (Form_NotesWidget) | View_ImageList、Button_NotesAdd |
| 书签面板 (Form_BookMarkWidget) | View_ImageList、Button_BookmarkAdd |
| 目录面板 (Form_CatalogWidget) | View_CatalogTree |
| 缩略图面板 (Form_ThumbnailWidget) | View_ImageList、pageEdit、Button_ThumbnailPre、Button_ThumbnailNext |
| 文档标签页栏 (Form_DocTabBar) | 多数字员工并行工作站.pdf、向左滚动、向右滚动、DTabBarAddButton |
| 初始导航页 (Form_CentralNavPage) | Button_SelectFile、Label_Icon |
| 文档阅读区 (Form_CentralDocPage) | Form_DocTabBar、Form_SheetSidebar |
| 主菜单 (DTitlebarMainMenu) | 新窗口、新标签页、保存、另存为、在文件管理器中显示、放大镜、工具、搜索、打印、主题、帮助、关于、退出 |
| 工具子菜单 (Menu_Hand) | 选择工具、手形工具 |
| 主题子菜单 (DTitlebarThemeMenu) | 浅色、深色、跟随系统 |
| 手形/选择工具菜单 (Menu_Hand) | 选择工具、手形工具 |
| 初始界面 (Form_CentralNavPage) | Button_SelectFile、Label_Icon |
| 加密页 (EncryptionPage) | passwdEdit、ensureBtn |
| 注释文本编辑 (TextEditShadowWidget / TransparentTextEdit) | 注释输入框（文字编辑区） |

## 表3：功能-操作-断言映射

| 功能 | 操作 | 断言目标元素 |
| --- | --- | --- |
| 打开文件 | 点击 Button_SelectFile | Button_SelectFile |
| 初始界面图标 | 查看初始界面 | Label_Icon |
| 切换缩略图视图 | 点击 Button_ThumbnailToggle 或 Button_thumbnail | View_ImageList |
| 切换目录视图 | 点击 Button_catalog | View_CatalogTree |
| 切换书签视图 | 点击 Button_bookmark | Button_BookmarkAdd |
| 切换注释视图 | 点击 Button_annotation | Button_NotesAdd |
| 切换搜索视图 | 点击 Button_search | View_ImageList |
| 添加书签 | 点击 Button_BookmarkAdd | Button_BookmarkAdd |
| 添加注释 | 点击 Button_NotesAdd | Button_NotesAdd |
| 目录导航 | 点击 View_CatalogTree 中节点 | View_CatalogTree |
| 上一页 | 点击 Button_ThumbnailPre | pageEdit |
| 下一页 | 点击 Button_ThumbnailNext | pageEdit |
| 跳转页码 | 在 pageEdit 输入页码并按 Enter | pageEdit |
| 缩放比例输入 | 在 DLineEditChildLineEdit 输入比例 | DLineEditChildLineEdit |
| 放大页面 | 主菜单 → 放大镜 / 标题栏 + | DLineEditChildLineEdit |
| 缩小页面 | 标题栏 - | DLineEditChildLineEdit |
| 调整页面视图 | 在缩放下拉框选择 双页显示/默认大小/适合页面/适应高度/适应宽度 | DLineEditChildLineEdit |
| 新增标签页 | 点击 DTabBarAddButton | DTabBarAddButton |
| 新增标签页（菜单） | 主菜单 → 新标签页 | DTabBarAddButton |
| 新增窗口（菜单） | 主菜单 → 新窗口 | DTitlebarDWindowOptionButton |
| 切换标签页 | 点击 多数字员工并行工作站.pdf (page tab) | 多数字员工并行工作站.pdf |
| 标签栏左滚 | 点击 向左滚动 | 向左滚动 |
| 标签栏右滚 | 点击 向右滚动 | 向右滚动 |
| 标签排序/拖拽 | 拖拽标签页 | 多数字员工并行工作站.pdf |
| 最小化窗口 | 点击 DTitlebarDWindowMinButton | DTitlebarDWindowMinButton |
| 最大化窗口 | 点击 DTitlebarDWindowMaxButton | DTitlebarDWindowMaxButton |
| 关闭窗口 | 点击 DTitlebarDWindowCloseButton | DTitlebarDWindowCloseButton |
| 退出应用 | 主菜单 → 退出 | 退出 |
| 全屏切换 | 主菜单相关 / F11 | DTitlebarDWindowQuitFullscreenButton |
| 保存文件 | 主菜单 → 保存 | 保存 |
| 另存为 | 主菜单 → 另存为 | 另存为 |
| 定位文件 | 主菜单 → 在文件管理器中显示 | 在文件管理器中显示 |
| 文档信息 | 右键菜单 → 文档信息 | dtk_context_menu |
| 打印 | 主菜单 → 打印 | 打印 |
| 搜索文档 | 主菜单 → 搜索 / Ctrl+F | Button_search |
| 搜索结果高亮 | 在搜索框输入关键字并按 Enter | View_ImageList |
| 选择工具 | 主菜单 → 工具 → 选择工具 / 右键菜单 选择工具 | 选择工具 |
| 手形工具 | 主菜单 → 工具 → 手形工具 / 右键菜单 手形工具 | 手形工具 |
| 切换主题 | 主菜单 → 主题 → 浅色/深色/跟随系统 | 浅色 |
| 帮助 | 主菜单 → 帮助 | 帮助 |
| 关于 | 主菜单 → 关于 | 关于 |
| 加密文档输入密码 | 在 EncryptionPage 的 passwdEdit 输入密码 | passwdEdit |
| 加密文档确认 | 点击 EncryptionPage 的 ensureBtn | ensureBtn |
| 高亮文本 | 右键菜单 → 高亮 | dtk_context_menu |
| 取消高亮 | 右键菜单 → 取消高亮 | dtk_context_menu |
| 左旋转 | 右键菜单 → 左旋转 | dtk_context_menu |
| 右旋转 | 右键菜单 → 右旋转 | dtk_context_menu |
| 添加注释（文本） | 右键菜单 → 添加注释 | dtk_context_menu |
| 删除注释 | 右键菜单 → 删除注释 | dtk_context_menu |
| 复制注释 | 右键菜单 → 复制 | dtk_context_menu |
| 上一页/下一页（右键） | 右键菜单 → 前一页/后一页/第一页/最后一页 | dtk_context_menu |
