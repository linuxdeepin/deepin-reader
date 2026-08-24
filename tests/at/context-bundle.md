# context-bundle.md

> 生成依据：at-tree-annotated.yaml + docs/modules/*.md（不含 ui-map.md，未就绪）。
> 功能描述以 docs（基本功能/常用操作/主菜单/使用入门）为依据；comment 字段为空、依树结构与文档推断者标注"（推断）"。
> 元素名与 at-tree-annotated.yaml 的 name 字段完全一致。两个同名窗口（Form_DMainWindow / Form_DMainWindow_1）为同一界面的两次抓取，同名字段合并为一行。

## 元素-功能对照表

| 元素名 | Role | 功能描述 | 可见条件 |
|--------|------|----------|----------|
| 新窗口 | menu item | 打开新窗口创建第二个文档查看器窗口（主菜单） | 主菜单打开时 |
| 新标签页 | menu item | 新增文档标签页（基本功能-管理标签页） | 主菜单打开时 |
| 保存 | menu item | 保存当前编辑/修改的文档（快捷键 Ctrl+S） | 主菜单打开时 |
| 另存为 | menu item | 将当前文档另存为新文件（快捷键 Ctrl+Shift+S） | 主菜单打开时 |
| 在文件管理器中显示 | menu item | 定位当前文件在文件管理器中的存储位置（主菜单） | 主菜单打开时；文档打开时 |
| 放大镜 | menu item | 放大鼠标所指页面区域（Esc/右键取消）（主菜单） | 主菜单打开时 |
| 工具 | menu item | 二级菜单入口：切换选择工具/手形工具（主菜单） | 主菜单打开时 |
| 选择工具 | menu item | 切换为选择工具，支持选择文本内容（工具→选择工具） | 工具二级菜单打开时 |
| 手形工具 | menu item | 切换为手形工具，支持在文档中移动（工具→手形工具） | 工具二级菜单打开时 |
| 搜索 | menu item | 打开搜索窗口（快捷键 Ctrl+F）（常用操作-搜索） | 主菜单打开时 |
| 打印 | menu item | 打开打印预览并打印文档（快捷键 Ctrl+P）（基本功能-打印文件） | 主菜单打开时；文档打开时 |
| 主题 | menu item | 二级菜单：切换窗口主题（浅色/深色/跟随系统）（主菜单-主题） | 主菜单打开时 |
| 浅色 | menu item | 应用浅色主题（主题） | 主题二级菜单打开时 |
| 深色 | menu item | 应用深色主题（主题） | 主题二级菜单打开时 |
| 跟随系统 | menu item | 主题跟随系统设置（主题） | 主题二级菜单打开时 |
| 帮助 | menu item | 查看文档查看器帮助手册（主菜单-帮助） | 主菜单打开时 |
| 关于 | menu item | 查看版本与介绍信息（主菜单-关于） | 主菜单打开时 |
| 退出 | menu item | 退出文档查看器（主菜单-退出 / 使用入门-关闭文档查看器） | 主菜单打开时 |
| DTitlebarIconLabel | button | 标题栏左侧应用图标/菜单入口，点击展开主菜单 | 主窗口标题栏（始终） |
| Button_ThumbnailToggle | check box | 切换缩略图侧边栏显示/隐藏（标题栏） | 标题栏（始终） |
| DLineEditChildLineEdit | text | 标题栏缩放比例输入框，输入/下拉选择缩放比例（常用操作-放大和缩小、调整页面视图） | 标题栏（始终） |
| DTitlebarDWindowOptionButton | button | 标题栏菜单按钮（更多选项） | 标题栏（始终） |
| DTitlebarDWindowMinButton | button | 最小化窗口 | 标题栏（始终） |
| DTitlebarDWindowMaxButton | button | 最大化窗口 | 标题栏（始终） |
| DTitlebarDWindowQuitFullscreenButton | button | 退出全屏按钮 | 全屏状态下的标题栏 |
| DTitlebarDWindowCloseButton | button | 关闭窗口/退出文档查看器（使用入门-关闭） | 标题栏（始终） |
| Button_SelectFile | button | 初始界面"选择文件"按钮，打开文件对话框选择文件（基本功能-打开文件） | 初始（未打开文档）界面 |
| Button_SelectFile_1 | button | 选择文件按钮（第二窗口 Form_DMainWindow_1，同 Button_SelectFile） | 第二窗口初始界面 |
| 多数字员工并行工作站.pdf | page tab list | 文档标签页列表，含当前打开的文档标签（基本功能-管理标签页） | 文档打开时 |
| 多数字员工并行工作站.pdf | page tab | 当前文档标签页，点击可切换/激活对应文档 | 文档打开、多标签时 |
| 向左滚动 | button | 标签页过多时向左滚动标签（基本功能-管理标签页） | 文档打开、标签过多时 |
| 向右滚动 | button | 标签页过多时向右滚动标签（基本功能-管理标签页） | 文档打开、标签过多时 |
| DTabBarAddButton | button | 标签栏添加新标签按钮 +（基本功能-管理标签页） | 两个及以上标签时 |
| Button_NotesAdd | button | 注释目录"添加注释"按钮（管理注释） | 侧边栏注释目录显示时 |
| Button_BookmarkAdd | button | 书签目录"添加书签"按钮（管理书签） | 侧边栏书签目录显示时 |
| View_CatalogTree | tree | 文档目录树，点击条目跳转对应章节（切换目录/文档目录） | 侧边栏目录显示时 |
| pageEdit | text | 缩略图目录页码输入框，输入页码按 Enter 跳转（常用操作-快速翻页） | 侧边栏缩略图目录显示时 |
| Button_ThumbnailPre | button | 缩略图上一页 | 侧边栏缩略图目录显示时 |
| Button_ThumbnailNext | button | 缩略图下一页 | 侧边栏缩略图目录显示时 |
| Button_thumbnail | check box | 侧边栏切换缩略图目录（切换目录） | 侧边栏显示时 |
| Button_catalog | check box | 侧边栏切换文档目录（切换目录） | 侧边栏显示时 |
| Button_bookmark | check box | 侧边栏切换书签目录（切换目录） | 侧边栏显示时 |
| Button_annotation | check box | 侧边栏切换注释目录（切换目录/管理注释） | 侧边栏显示时 |
| Button_search | check box | 侧边栏切换搜索结果目录（搜索）（推断） | 侧边栏显示时 |
| Form_SearchResWidget | form | 搜索结果显示容器（搜索→侧边栏显示全部搜索结果）（常用操作-搜索） | 执行搜索后
| Form_NotesWidget | form | 注释容器/注释列表 | 打开注释目录时 |
| Form_BookMarkWidget | form | 书签内容容器 | 打开书签目录时 |
| Form_CatalogWidget | form | 文档目录容器 | 打开文档目录时 |
| Form_ThumbnailWidget | form | 缩略图内容容器 | 打开缩略图目录时 |
| View_ImageList | list | 侧边栏缩略图/搜索/书签/注释图片列表 | 对应侧边栏目录显示时 |
| Form_DocSheet | form | 文档内容显示区域（旋转/高亮/缩放/翻页效果） | 文档打开时 |
| Form_SheetSidebar | form | 左侧侧边栏容器（缩略图/目录/书签/注释/搜索） | 侧边栏显示时 |
| Form_CentralNavPage | form | 导航页（初始界面，"选择文件"所在） | 未打开文档时 |
| Form_CentralDocPage | form | 文档页（标签栏+内容） | 文档打开时 |

## 界面-元素映射

| 界面 | 包含元素 |
|------|----------|
| 主窗口（Form_DMainWindow） | Form_DMainWindowTitlebar, Form_Central, Form_CentralNavPage, Form_CentralDocPage |
| 初始界面（Form_CentralNavPage） | Button_SelectFile, Label_Icon |
| 标题栏-左侧（Form_DTitlebarLeftArea） | DTitlebarIconLabel, Button_ThumbnailToggle, Form_scaleEdit_P(含 DLineEditChildLineEdit) |
| 标题栏-右侧（Form_DTitlebarButtonArea） | DTitlebarDWindowOptionButton, DTitlebarDWindowMinButton, DTitlebarDWindowMaxButton, DTitlebarDWindowQuitFullscreenButton, DTitlebarDWindowCloseButton |
| 标签栏（Form_DocTabBar） | 多数字员工并行工作站.pdf(page tab list/tab), 向左滚动, 向右滚动, DTabBarAddButton |
| 文档区（Form_DocSheet） | Form_SheetSidebar, Form_CentralDocPage 内容区 |
| 侧边栏（Form_SheetSidebar） | Button_thumbnail, Button_catalog, Button_bookmark, Button_annotation, Button_search |
| 缩略图目录（Form_ThumbnailWidget） | View_ImageList, Form_Paging, pageEdit, Button_ThumbnailPre, Button_ThumbnailNext |
| 文档目录（Form_CatalogWidget） | View_CatalogTree |
| 书签目录（Form_BookMarkWidget） | View_ImageList, Button_BookmarkAdd |
| 注释目录（Form_NotesWidget） | View_ImageList, Button_NotesAdd |
| 搜索结果（Form_SearchResWidget） | View_ImageList |
| 主菜单（DTitlebarMainMenu） | 新窗口, 新标签页, 保存, 另存为, 在文件管理器中显示, 放大镜, 工具, 搜索, 打印, 主题, 帮助, 关于, 退出 |
| 主菜单-工具子菜单（Menu_Hand） | 选择工具, 手形工具 |
| 主菜单-主题子菜单（DTitlebarThemeMenu） | 浅色, 深色, 跟随系统 |
| 第二窗口（Form_DMainWindow_1） | Button_SelectFile_1, DTitlebarIconLabel, Button_ThumbnailToggle, DTitlebarDWindow*（同第一窗口） |

## 功能-操作-断言映射

| 功能 | 操作 | 断言目标 |
|------|------|----------|
| 打开文件 | 初始界面点击选择文件 → 文件对话框选择文件 | Button_SelectFile |
| 打开文件后用缩略图目录 | 初始界面点击选择文件打开文档 | Form_CentralDocPage |
| 保存文件 | 主菜单 → 保存（Ctrl+S） | 保存 |
| 另存为文件 | 主菜单 → 另存为（Ctrl+Shift+S） | 另存为 |
| 新建窗口 | 主菜单 → 新窗口 | 新窗口 |
| 新建标签页 | 主菜单 → 新标签页 或点击 DTabBarAddButton | 新标签页 |
| 在文件管理器中显示 | 主菜单 → 在文件管理器中显示 | 在文件管理器中显示 |
| 打开放大镜 | 主菜单 → 放大镜 | 放大镜 |
| 切换选择工具 | 主菜单 → 工具 → 选择工具 | 选择工具 |
| 切换手形工具 | 主菜单 → 工具 → 手形工具 | 手形工具 |
| 打开搜索 | 主菜单 → 搜索（或 Ctrl+F） | 搜索 |
| 搜索并查看结果 | 在搜索框输入关键字按 Enter → 侧边栏显示结果 | Form_SearchResWidget |
| 打印文档 | 主菜单 → 打印（或 Ctrl+P） | 打印 |
| 切换深色主题 | 主菜单 → 主题 → 深色 | 深色 |
| 切换浅色主题 | 主菜单 → 主题 → 浅色 | 浅色 |
| 主题跟随系统 | 主菜单 → 主题 → 跟随系统 | 跟随系统 |
| 打开帮助手册 | 主菜单 → 帮助 | 帮助 |
| 查看关于信息 | 主菜单 → 关于 | 关于 |
| 退出文档查看器 | 主菜单 → 退出 | 退出 |
| 关闭窗口 | 点击标题栏关闭按钮 | DTitlebarDWindowCloseButton |
| 最小化窗口 | 点击标题栏最小化按钮 | DTitlebarDWindowMinButton |
| 最大化窗口 | 点击标题栏最大化按钮 | DTitlebarDWindowMaxButton |
| 打开主菜单 | 点击标题栏图标 或 DTitlebarDWindowOptionButton | DTitlebarDWindowOptionButton |
| 退出全屏 | 全屏时点击标题栏退出全屏按钮 | DTitlebarDWindowQuitFullscreenButton |
| 切换缩略图侧边栏 | 点击标题栏缩略图开关 | Button_ThumbnailToggle |
| 放大页面 | 标题栏缩放输入框输入更大比例（DLineEditChildLineEdit） | DLineEditChildLineEdit |
| 缩小页面 | 标题栏缩放输入框输入更小比例 | DLineEditChildLineEdit |
| 设为适合页面 | 标题栏缩放下拉框选择适合页面 | DLineEditChildLineEdit |
| 设为适应宽度 | 标题栏缩放下拉框选择适合宽度 | DLineEditChildLineEdit |
| 设为适应高度 | 标题栏缩放下拉框选择适合高度 | DLineEditChildLineEdit |
| 设为默认大小 | 标题栏缩放下拉框选择默认大小 | DLineEditChildLineEdit |
| 切换双页显示 | 标题栏缩放下拉框选择双页显示 | DLineEditChildLineEdit |
| 切换标签页 | 点击标签页"多数字员工并行工作站.pdf" | 多数字员工并行工作站.pdf |
| 添加新标签 | 点击 DTabBarAddButton | DTabBarAddButton |
| 滚动标签左移 | 点击向左滚动按钮 | 向左滚动 |
| 滚动标签右移 | 点击向右滚动按钮 | 向右滚动 |
| 显示缩略图目录 | 点击侧边栏 Button_Thumbnail | Button_thumbnail |
| 显示文档目录 | 点击侧边栏 Button_Catalog | Button_catalog |
| 显示书签目录 | 点击侧边栏 Button_Bookmark | Button_bookmark |
| 显示注释目录 | 点击侧边栏 Button_Annotation | Button_annotation |
| 显示搜索目录 | 点击侧边栏 Button_Search | Button_search |
| 跳转到指定页面 | 在缩略图目录页码输入框输入页码按 Enter | pageEdit |
| 缩略图上一页 | 点击按钮 | Button_ThumbnailPre |
| 缩略图下一页 | 点击按钮 | Button_ThumbnailNext |
| 按目录跳转章节 | 点击目录树条目（View_CatalogTree） | View_CatalogTree |
| 添加书签 | 打开书签目录点击添加按钮（或右键→添加书签） | Button_BookmarkAdd |
| 添加注释 | 注释目录点击添加注释（或右键→添加注释） | Button_NotesAdd |
| 旋转文档（左/右旋转） | 右键 → 左旋转 | Form_DocSheet |
| 高亮文本 | 选中文本右键 → 高亮 | Form_DocSheet |
| 取消高亮 | 选中高亮文本右键 → 取消高亮 | Form_DocSheet |
| 退出全屏（Esc/F11） | 全屏状态下按 Esc 或 F11 | Form_DocSheet |