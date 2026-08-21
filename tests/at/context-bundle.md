# deepin-reader AT-SPI 上下文包

## 1. 元素-功能对照表

| 元素名 | Role | 功能描述 | 可见条件 |
|--------|------|---------|---------|
| DTitlebarIconLabel | button | 打开应用菜单/窗口图标 | 始终 |
| Button_ThumbnailToggle | check box | 切换侧栏缩略图面板显示 | 文档打开后 |
| DLineEditChildLineEdit | text | 输入缩放比例数值 | 文档打开后 |
| DTitlebarDWindowOptionButton | button | 打开标题栏菜单 | 始终 |
| DTitlebarDWindowMinButton | button | 最小化窗口 | 始终 |
| DTitlebarDWindowMaxButton | button | 最大化/还原窗口 | 始终 |
| DTitlebarDWindowQuitFullscreenButton | button | 退出全屏模式 | 全屏模式 |
| DTitlebarDWindowCloseButton | button | 关闭窗口 | 始终 |
| 向左滚动 | button | 向左滚动文档标签页 | 标签页溢出时 |
| 向右滚动 | button | 向右滚动文档标签页 | 标签页溢出时 |
| DTabBarAddButton | button | 添加新文档标签页 | 始终 |
| Button_SelectFile | button | 选择文件打开 | 导航页显示时 |
| 新窗口 | menu item | 打开新阅读器窗口 | 始终 |
| 新标签页 | menu item | 在当前窗口打开新标签页 | 始终 |
| 保存 | menu item | 保存当前文档的更改 | 文档有未保存更改 |
| 另存为 | menu item | 另存当前文档 | 始终 |
| 在文件管理器中显示 | menu item | 在文件管理器中显示文档位置 | 始终 |
| 放大镜 | menu item | 切换放大镜模式 | 始终 |
| 工具 | menu item | 打开工具子菜单 | 文档打开后 |
| 选择工具 | menu item | 切换到选择文本工具 | 文档打开后 |
| 手形工具 | menu item | 切换到手形工具 | 文档打开后 |
| 搜索 | menu item | 在文档中搜索文本 | PDF/DOCX/XPS文档 |
| 打印 | menu item | 打印当前文档 | 始终 |
| 主题 | menu item | 打开主题子菜单 | 始终 |
| 浅色 | menu item | 切换为浅色主题 | 始终 |
| 深色 | menu item | 切换为深色主题 | 始终 |
| 跟随系统 | menu item | 跟随系统主题 | 始终 |
| 帮助 | menu item | 查看帮助文档 | 始终 |
| 关于 | menu item | 查看关于信息 | 始终 |
| 退出 | menu item | 退出应用程序 | 始终 |

## 2. 界面-元素映射

| 界面 | 包含元素 |
|------|---------|
| 主窗口 (Form_DMainWindow) | DTitlebarIconLabel, Button_ThumbnailToggle, DLineEditChildLineEdit, DTitlebarDWindowOptionButton, DTitlebarDWindowMinButton, DTitlebarDWindowMaxButton, DTitlebarDWindowQuitFullscreenButton, DTitlebarDWindowCloseButton, 向左滚动, 向右滚动, DTabBarAddButton, Button_SelectFile |
| 标题栏左侧 (Form_DTitlebarLeftArea) | DTitlebarIconLabel |
| 标题栏标题区 (Form_TitleWidget) | Button_ThumbnailToggle, DLineEditChildLineEdit |
| 缩放编辑区 (Form_scaleEdit_P) | DLineEditChildLineEdit |
| 标题栏右侧按钮区 (Form_DTitlebarButtonArea) | DTitlebarDWindowOptionButton, DTitlebarDWindowMinButton, DTitlebarDWindowMaxButton, DTitlebarDWindowQuitFullscreenButton, DTitlebarDWindowCloseButton |
| 文档标签栏 (Form_DocTabBar) | 向左滚动, 向右滚动, DTabBarAddButton |
| 导航页 (Form_CentralNavPage) | Button_SelectFile |
| 标题栏菜单 (DTitlebarMainMenu) | 新窗口, 新标签页, 保存, 另存为, 在文件管理器中显示, 放大镜, 工具, 搜索, 打印, 主题, 帮助, 关于, 退出 |
| 工具子菜单 (Menu_Hand) | 选择工具, 手形工具 |
| 主题子菜单 (DTitlebarThemeMenu) | 浅色, 深色, 跟随系统 |

## 3. 功能-操作-断言映射

| 功能 | 操作 | 断言目标 |
|------|------|---------|
| 启动应用显示主窗口 | 启动应用 | DTitlebarDWindowOptionButton |
| 打开文件 | 点击Button_SelectFile | Button_ThumbnailToggle |
| 切换缩略图面板 | 点击Button_ThumbnailToggle | Button_ThumbnailToggle |
| 输入缩放比例 | 点击DLineEditChildLineEdit, 输入数值 | DLineEditChildLineEdit |
| 最小化窗口 | 点击DTitlebarDWindowMinButton | DTitlebarDWindowMinButton |
| 最大化窗口 | 点击DTitlebarDWindowMaxButton | DTitlebarDWindowMaxButton |
| 关闭窗口 | 点击DTitlebarDWindowCloseButton | DTitlebarDWindowCloseButton |
| 退出全屏模式 | 点击DTitlebarDWindowQuitFullscreenButton | DTitlebarDWindowQuitFullscreenButton |
| 打开标题栏菜单 | 点击DTitlebarDWindowOptionButton | DTitlebarDWindowOptionButton |
| 打开新窗口 | 打开标题栏菜单, 点击新窗口 | 新窗口 |
| 打开新标签页 | 打开标题栏菜单, 点击新标签页 | 新标签页 |
| 保存文档 | 打开标题栏菜单, 点击保存 | 保存 |
| 另存文档 | 打开标题栏菜单, 点击另存为 | 另存为 |
| 在文件管理器显示 | 打开标题栏菜单, 点击在文件管理器中显示 | 在文件管理器中显示 |
| 切换放大镜模式 | 打开标题栏菜单, 点击放大镜 | 放大镜 |
| 打开工具子菜单 | 打开标题栏菜单, 点击工具 | 工具 |
| 切换到选择工具 | 打开标题栏菜单, 工具, 点击选择工具 | 选择工具 |
| 切换到手形工具 | 打开标题栏菜单, 工具, 点击手形工具 | 手形工具 |
| 搜索文档 | 打开标题栏菜单, 点击搜索 | 搜索 |
| 打印文档 | 打开标题栏菜单, 点击打印 | 打印 |
| 切换浅色主题 | 打开标题栏菜单, 主题, 点击浅色 | 浅色 |
| 切换深色主题 | 打开标题栏菜单, 主题, 点击深色 | 深色 |
| 跟随系统主题 | 打开标题栏菜单, 主题, 点击跟随系统 | 跟随系统 |
| 查看帮助 | 打开标题栏菜单, 点击帮助 | 帮助 |
| 查看关于信息 | 打开标题栏菜单, 点击关于 | 关于 |
| 退出应用 | 打开标题栏菜单, 点击退出 | 退出 |
| 添加新标签页 | 点击DTabBarAddButton | DTabBarAddButton |
| 向左滚动标签页 | 点击向左滚动 | 向左滚动 |
| 向右滚动标签页 | 点击向右滚动 | 向右滚动 |