# Context Bundle — Deepin Reader AT-SPI

## 1. 元素-功能对照表

| 元素名 | Role | 功能描述 | 可见条件 |
|--------|------|---------|---------|
| DTitlebarIconLabel | button | 应用图标按钮，点击打开主菜单 | 始终 |
| Button_ThumbnailToggle | check box | 切换缩略图显示/隐藏 (标题栏) | 始终 |
| DLineEditChildLineEdit | text | 缩放比例输入框，可输入百分比数值调整页面缩放 | 始终 |
| DTitlebarDWindowOptionButton | button | 主菜单按钮 (DTK窗口选项) | 始终 |
| DTitlebarDWindowMinButton | button | 最小化窗口 | 始终 |
| DTitlebarDWindowMaxButton | button | 最大化/还原窗口 | 非全屏时 |
| DTitlebarDWindowQuitFullscreenButton | button | 退出全屏模式 | 全屏时 |
| DTitlebarDWindowCloseButton | button | 关闭窗口/退出应用 | 始终 |
| '' (无名page tab list) | page tab list | 标签页列表区域，包含已打开的文档标签 | 有文档时 |
| 向左滚动 | button | 标签页向左滚动（标签页过多时显示） | 有文档时且标签页超出可见区域 |
| 向右滚动 | button | 标签页向右滚动（标签页过多时显示） | 有文档时且标签页超出可见区域 |
| DTabBarAddButton | button | 添加新标签页 | 有文档时且标签页数量 >= 2 |
| Button_SelectFile | button | 选择文件按钮，打开文件选择对话框 | 无文档时（导航页可见） |
| 新窗口 | menu item | 新建一个文档查看器窗口 | 主菜单打开时 |
| 新标签页 | menu item | 在当前窗口新建标签页 | 主菜单打开时 |
| 保存 | menu item | 保存当前文档 | 主菜单打开时且文档已打开 |
| 另存为 | menu item | 另存当前文档 | 主菜单打开时且文档已打开 |
| 在文件管理器中显示 | menu item | 在文件管理器中定位当前文档 | 主菜单打开时且文档已打开 |
| 放大镜 | menu item | 启用放大镜功能，放大查看页面区域 | 主菜单打开时且文档已打开 |
| 工具 | menu item | 打开工具子菜单（选择工具/手形工具） | 主菜单打开时 |
| 选择工具 | menu item | 切换为选择文本工具（主菜单-工具子菜单内） | 工具子菜单打开时且文档已打开 |
| 手形工具 | menu item | 切换为手形工具，拖动移动文档（主菜单-工具子菜单内） | 工具子菜单打开时且文档已打开 |
| 搜索 | menu item | 打开搜索栏 | 主菜单打开时且文档已打开 |
| 打印 | menu item | 打印当前文档 | 主菜单打开时且文档已打开 |
| 主题 | menu item | 打开主题子菜单 | 主菜单打开时 |
| 浅色 | menu item | 切换为浅色主题（主菜单-主题子菜单内） | 主题子菜单打开时 |
| 深色 | menu item | 切换为深色主题（主菜单-主题子菜单内） | 主题子菜单打开时 |
| 跟随系统 | menu item | 切换为跟随系统主题（主菜单-主题子菜单内） | 主题子菜单打开时 |
| 帮助 | menu item | 查看帮助手册 | 主菜单打开时 |
| 关于 | menu item | 查看版本和介绍信息 | 主菜单打开时 |
| 退出 | menu item | 退出文档查看器 | 主菜单打开时 |
| 选择工具 | menu item | 切换为选择文本工具（独立工具菜单内） | 工具菜单弹出时且文档已打开 |
| 手形工具 | menu item | 切换为手形工具（独立工具菜单内） | 工具菜单弹出时且文档已打开 |
| 浅色 | menu item | 切换为浅色主题（独立主题菜单内） | 主题菜单弹出时 |
| 深色 | menu item | 切换为深色主题（独立主题菜单内） | 主题菜单弹出时 |
| 跟随系统 | menu item | 切换为跟随系统主题（独立主题菜单内） | 主题菜单弹出时 |

## 2. 界面-元素映射

| 界面 | 包含元素 |
|------|---------|
| 主窗口标题栏-左侧区域 (Form_DTitlebarLeftArea) | DTitlebarIconLabel, Button_ThumbnailToggle, DLineEditChildLineEdit |
| 主窗口标题栏-右侧区域 (Form_DTitlebarRightArea) | DTitlebarDWindowOptionButton, DTitlebarDWindowMinButton, DTitlebarDWindowMaxButton, DTitlebarDWindowQuitFullscreenButton, DTitlebarDWindowCloseButton |
| 导航页-无文档时 (Form_CentralNavPage) | Button_SelectFile |
| 标签栏 (Form_DocTabBar) | (无名 page tab list), 向左滚动, 向右滚动, DTabBarAddButton |
| 主菜单 (DTitlebarMainMenu) | 新窗口, 新标签页, 保存, 另存为, 在文件管理器中显示, 放大镜, 工具, 搜索, 打印, 主题, 帮助, 关于, 退出 |
| 工具子菜单 (Menu_Hand，主菜单内) | 选择工具, 手形工具 |
| 主题子菜单 (DTitlebarThemeMenu，主菜单内) | 浅色, 深色, 跟随系统 |
| 工具菜单 (Menu_Hand，独立弹出) | 选择工具, 手形工具 |
| 主题菜单 (DTitlebarThemeMenu，独立弹出) | 浅色, 深色, 跟随系统 |

## 3. 功能-操作-断言映射

| 功能 | 操作 | 断言目标 |
|------|------|---------|
| 打开文件 | 在导航页点击"选择文件"按钮或拖拽文件到窗口 | Button_SelectFile |
| 新建窗口 | 主菜单 → 新窗口 或 快捷键 Ctrl+N | 新窗口 |
| 新建标签页 | 主菜单 → 新标签页 或 快捷键 Ctrl+T 或 点击添加标签按钮 | 新标签页 |
| 保存文件 | 主菜单 → 保存 或 快捷键 Ctrl+S | 保存 |
| 另存文件 | 主菜单 → 另存为 或 快捷键 Ctrl+Shift+S | 另存为 |
| 在文件管理器中显示 | 主菜单 → 在文件管理器中显示 | 在文件管理器中显示 |
| 启用放大镜 | 主菜单 → 放大镜 或 快捷键 Ctrl+M | 放大镜 |
| 切换选择工具 | 主菜单 → 工具 → 选择工具 或 快捷键 Alt+Z | 选择工具 |
| 切换手形工具 | 主菜单 → 工具 → 手形工具 或 快捷键 Alt+H | 手形工具 |
| 打开搜索栏 | 主菜单 → 搜索 或 快捷键 Ctrl+F | 搜索 |
| 打印文档 | 主菜单 → 打印 或 快捷键 Ctrl+P 或 右键菜单 → 打印 | 打印 |
| 切换浅色主题 | 主菜单 → 主题 → 浅色 | 浅色 |
| 切换深色主题 | 主菜单 → 主题 → 深色 | 深色 |
| 切换跟随系统主题 | 主菜单 → 主题 → 跟随系统 | 跟随系统 |
| 查看帮助手册 | 主菜单 → 帮助 | 帮助 |
| 查看关于信息 | 主菜单 → 关于 | 关于 |
| 退出应用 | 主菜单 → 退出 或 点击关闭按钮 | 退出 |
| 最小化窗口 | 点击标题栏最小化按钮 | DTitlebarDWindowMinButton |
| 最大化/还原窗口 | 点击标题栏最大化按钮 | DTitlebarDWindowMaxButton |
| 退出全屏 | 点击标题栏退出全屏按钮 或 Esc 键 | DTitlebarDWindowQuitFullscreenButton |
| 关闭窗口 | 点击标题栏关闭按钮 | DTitlebarDWindowCloseButton |
| 切换缩略图显示 | 点击标题栏缩略图切换按钮 | Button_ThumbnailToggle |
| 输入缩放比例 | 在标题栏缩放输入框中输入百分比数值后回车 | DLineEditChildLineEdit |
| 标签页向左滚动 | 点击标签栏左滚动按钮 | 向左滚动 |
| 标签页向右滚动 | 点击标签栏右滚动按钮 | 向右滚动 |
| 添加新标签页 | 点击标签栏添加标签按钮 (两个以上标签时可见) | DTabBarAddButton |
| 切换标签页 | 点击标签栏中的标签页 | (无名 page tab list) |
| 打开主菜单 | 点击标题栏应用图标或主菜单按钮 | DTitlebarIconLabel |
| 打开主菜单(DTK选项按钮) | 点击标题栏选项按钮 | DTitlebarDWindowOptionButton |