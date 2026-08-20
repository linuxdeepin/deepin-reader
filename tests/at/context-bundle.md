# 上下文包 — deepin-reader

---

## 1. 元素-功能对照表

| 元素名 | Role | 功能描述 | 可见条件 |
|--------|------|----------|----------|
| DTitlebarIconLabel | button | 应用图标/标题 | 始终可见 |
| Button_ThumbnailToggle | check box | 显示/隐藏左侧栏缩略图面板 | 始终可见(文档打开后可用) |
| DLineEditChildLineEdit | text | 缩放百分比输入框 | 始终可见 |
| DTitlebarDWindowOptionButton | button | 打开标题栏主菜单 | 始终可见 |
| DTitlebarDWindowMinButton | button | 最小化窗口 | 始终可见 |
| DTitlebarDWindowMaxButton | button | 最大化/还原窗口 | 始终可见 |
| DTitlebarDWindowQuitFullscreenButton | button | 退出全屏 | 全屏可见 |
| DTitlebarDWindowCloseButton | button | 关闭窗口 | 始终可见 |
| (page tab list) | page tab list | 文档标签切换 | 有文档打开时 |
| 向左滚动 | button | 标签栏向左滚动 | 标签页过多时 |
| 向右滚动 | button | 标签栏向右滚动 | 标签页过多时 |
| DTabBarAddButton | button | 添加新标签(打开文档) | 有标签页时 |
| Label_Icon | image | 文档图标(导航页) | 无文档打开时 |
| Label_Drag documents here | label | "拖拽文档到此"提示文字 | 无文档打开时 |
| Label_format supported: ... | label | 支持格式提示文字 | 无文档打开时 |
| SelectFile | button | 选择文件按钮 | 无文档打开时 |
| Menu_Title | menu | 标题栏主菜单 | 点击选项按钮时瞬态 |
| 新窗口 | menu item | 打开新阅读窗口 | 主菜单展开时瞬态 |
| 新标签页 | menu item | 新标签页打开文档 | 主菜单展开时瞬态 |
| 保存 | menu item | 保存当前文档 | 主菜单展开时瞬态(有文档时可用) |
| 另存为 | menu item | 另存当前文档 | 主菜单展开时瞬态(有文档时可用) |
| 在文件管理器中显示 | menu item | 打开文件所在目录 | 主菜单展开时瞬态(有文档时可用) |
| 放大镜 | menu item | 开启放大镜工具 | 主菜单展开时瞬态(有文档时可用) |
| 工具 | menu item | 工具子菜单(选择/手型) | 主菜单展开时瞬态(有文档时可用) |
| 选择工具 | menu item | 切换为文本选择模式 | 工具菜单展开时瞬态 |
| 手形工具 | menu item | 切换为手形滚动模式 | 工具菜单展开时瞬态 |
| 搜索 | menu item | 打开文档内查找框 | 主菜单展开时瞬态(有文档时可用) |
| 打印 | menu item | 打印当前文档 | 主菜单展开时瞬态(有文档时可用) |
| 主题 | menu item | 主题子菜单(浅色/深色/跟随系统) | 主菜单展开时瞬态 |
| 浅色 | menu item | 切换浅色主题 | 主题菜单展开时瞬态 |
| 深色 | menu item | 切换深色主题 | 主题菜单展开时瞬态 |
| 跟随系统 | menu item | 跟随系统主题 | 主题菜单展开时瞬态 |
| 帮助 | menu item | 打开帮助文档 | 主菜单展开时瞬态 |
| 关于 | menu item | 显示关于信息 | 主菜单展开时瞬态 |
| 退出 | menu item | 退出应用 | 主菜单展开时瞬态 |
| Menu_Browser | menu | 文档区域右键菜单 | 右键文档区域时瞬态 |
| Menu_Hand | menu | 工具子菜单(右键菜单) | 右键菜单展开时瞬态 |
| Menu_BookMark | menu | 书签列表右键菜单 | 书签列表右键时瞬态 |
| Menu_Note | menu | 注释列表右键菜单 | 注释列表右键时瞬态 |
| Form_findSearchEdit_P | form | 查找输入框 | 查找框打开时 |
| Form_scaleEdit_P | form | 缩放输入框 | 始终可见 |
| Button_ThumbnailPre | button | 缩略图上一页 | 缩略图面板打开时(有文档) |
| Button_ThumbnailNext | button | 缩略图下一页 | 缩略图面板打开时(有文档) |
| Page/pageEdit | text | 跳转页输入 | 缩略图面板打开时(有文档) |
| Label_TotalPage | label | 总页数显示 | 缩略图面板打开时(有文档) |
| CurrentPage | label | 当前页显示 | 缩略图面板打开时(有文档) |
| View_ImageList | list | 侧栏列表(书签/注释/缩略图/搜索) | 对应侧栏视图打开时 |
| BookmarkAdd | button | 添加书签按钮 | 书签视图打开时 |
| NotesAdd | button | 添加注释按钮 | 注释视图打开时 |

## 2. 界面-元素映射

| 界面 | 包含元素 |
|------|----------|
| 标题栏 | DTitlebarIconLabel, Button_ThumbnailToggle, DLineEditChildLineEdit, Form_scaleEdit_P, DTitlebarDWindowOptionButton, DTitlebarDWindowMinButton, DTitlebarDWindowMaxButton, DTitlebarDWindowQuitFullscreenButton, DTitlebarDWindowCloseButton |
| 主菜单(瞬态) | Menu_Title, 新窗口, 新标签页, 保存, 另存为, 在文件管理器中显示, 放大镜, 工具, 选择工具, 手形工具, 搜索, 打印, 主题, 浅色, 深色, 跟随系统, 帮助, 关于, 退出 |
| 标签页栏(有文档) | (page tab list), 向左滚动, 向右滚动, DTabBarAddButton |
| 导航页(无文档) | Label_Icon, Label_Drag documents here, Label_format supported: ..., SelectFile |
| 文档区域 | SheetBrowser, Tips, Menu_Browser(瞬态), 选择工具, 手形工具, 主题子菜单 |
| 查找框(打开时) | Form_findSearchEdit_P, 上一个按钮, 下一个按钮, 关闭按钮 |
| 左侧栏—目录视图 | Label_title, View_CatalogTree |
| 左侧栏—书签视图 | View_ImageList, BookmarkAdd, Menu_BookMark(瞬态), BookMarkLine |
| 左侧栏—注释视图 | View_ImageList, NotesAdd, Menu_Note(瞬态), NotesLine |
| 左侧栏—缩略图视图 | View_ImageList, Paging, ThumbnailLine, Button_ThumbnailPre, Button_ThumbnailNext, Page, pageEdit, Label_TotalPage, CurrentPage |
| 左侧栏—搜索结果 | View_ImageList |
| 左侧栏—切换按钮 | Button_Catalog, Button_BookMark, Button_Note, Button_Thumbnail, Button_search |

## 3. 功能-操作-断言映射

| 功能 | 操作 | 断言目标 |
|------|------|----------|
| 打开文件(选择文件按钮) | element_action(SelectFile,click) 或 keyboard_hot_key(ctrl+o) | file_dialog_select → 文档打开后标签页可见 |
| 打开文件(拖拽) | (不可自动化) | — |
| 打开无文档首页 | 启动应用 | SelectFile 可见 |
| 关闭窗口 | element_action(DTitlebarDWindowCloseButton,click) | 窗口关闭 |
| 最小化 | element_action(DTitlebarDWindowMinButton,click) | 窗口最小化 |
| 全屏 | keyboard_press(F11) | DTitlebarDWindowQuitFullscreenButton 可见 |
| 退出全屏 | keyboard_press(F11) | DTitlebarDWindowQuitFullscreenButton 消失 |
| 切换缩略图面板 | element_action(Button_ThumbnailToggle,click) | 左侧栏缩略图视图出现 |
| 输入缩放比例 | keyboard_type(缩放输入框) | 缩放值变更 |
| 主菜单—新窗口 | dtk_main_menu(items:[新窗口]) | 新窗口出现 |
| 主菜单—新标签页 | dtk_main_menu(items:[新标签页]) | 新标签页 Tab 出现 |
| 主菜单—保存 | dtk_main_menu(items:[保存]) | (文档保存，无UI变化) |
| 主菜单—另存为 | dtk_main_menu(items:[另存为]) | file_dialog_select触发 |
| 主菜单—搜索 | dtk_main_menu(items:[搜索]) | Form_findSearchEdit_P 可见 |
| 主菜单—打印 | dtk_main_menu(items:[打印]) | 打印预览出现 |
| 主菜单—放大镜 | dtk_main_menu(items:[放大镜]) | 放大镜效果可见 |
| 主菜单—工具→选择文本 | dtk_main_menu(items:[工具→选择文本]) | 选择文本模式启用 |
| 主菜单—工具→手形工具 | dtk_main_menu(items:[工具→手形工具]) | 手形模式启用 |
| 主菜单—主题切换 | dtk_main_menu(items:[主题→浅色/深色/跟随系统]) | 界面主题变化 |
| 主菜单—帮助 | dtk_main_menu(items:[帮助]) | 帮助文档打开 |
| 主菜单—关于 | dtk_main_menu(items:[关于]) | 关于对话框出现 |
| 主菜单—退出 | dtk_main_menu(items:[退出]) | 窗口关闭 |
| 标签页—添加 | element_action(DTabBarAddButton,click) | 新标签页出现 |
| 标签页—切换 | element_action(标签,click) | 对应文档打开 |
| 标签页—关闭 | mouse_click(标签关闭按钮) | 标签页消失 |
| 侧栏—切换目录 | element_action(Button_Catalog,click) | CatalogWidget 可见 |
| 侧栏—切换书签 | element_action(Button_BookMark,click) | BookMarkWidget 可见 |
| 侧栏—添加书签 | element_action(BookmarkAdd,click) | 书签添加至列表 |
| 侧栏—切换注释 | element_action(Button_Note,click) | NotesWidget 可见 |
| 侧栏—添加注释 | element_action(NotesAdd,click) | 注释编辑框出现 |
| 侧栏—切换缩略图 | element_action(Button_Thumbnail,click) | ThumbnailWidget+Paging 可见 |
| 侧栏—翻页(缩略图) | element_action(Button_ThumbnailPre/Button_ThumbnailNext,click) | 缩略图翻页 |
| 侧栏—跳转页 | keyboard_type(pageEdit) + enter | 文档跳转 |
| 侧栏—搜索 | element_action(Button_search,click) | SearchResWidget 可见 |
| 查找—打开 | keyboard_hot_key(ctrl+f) 或 dtk_main_menu(items:[搜索]) | Form_findSearchEdit_P 可见 |
| 查找—输入文本 | keyboard_type(查找输入框) | 搜索结果 View_ImageList 更新 |
| 查找—关闭 | keyboard_press(Escape) 或 element_action(关闭按钮,click) | Form_findSearchEdit_P 消失 |
| 右键菜单—文档区域 | dtk_context_menu(selector:Menu_Browser,menu:[...]) | 菜单弹出 |
| 右键菜单—书签列表 | dtk_context_menu(selector:View_ImageList child_index:N, menu:[...]) | 菜单弹出 |
| 右键菜单—注释列表 | dtk_context_menu(selector:View_ImageList child_index:N, menu:[...]) | 菜单弹出 |
| 幻灯片播放 | keyboard_press(F5) | 幻灯片界面出现 |
| 快捷键显示 | keyboard_hot_key(ctrl+shift+/) | 快捷键对话框出现 |
| 加密文件打开 | 打开加密PDF → keyboard_type(密码) → element_action(确定按钮) | 文档正常打开 |