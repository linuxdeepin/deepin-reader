# Deepin Reader Context Bundle

> 生成时间: 2026-08-20
> 数据来源: at-tree-annotated.yaml + ui-map.md + expected-at-spi-elements.md + docs 帮助文档

---

## 1. 元素-功能对照表

|元素名|Role|功能描述|可见条件|
|------|----|--------|--------|
|DTitlebarIconLabel|button|应用图标（点击弹出窗口菜单）|始终|
|Button_ThumbnailToggle|check box|缩略图侧栏开关|始终|
|DLineEditChildLineEdit|text|缩放比例输入框|有文档打开|
|DTitlebarDWindowOptionButton|button|主菜单按钮|始终|
|DTitlebarDWindowMinButton|button|最小化窗口|始终|
|DTitlebarDWindowMaxButton|button|最大化/还原窗口|始终|
|DTitlebarDWindowQuitFullscreenButton|button|退出全屏|全屏状态|
|DTitlebarDWindowCloseButton|button|关闭窗口|始终|
|向左滚动|button|向左滚动标签页|标签页溢出|
|向右滚动|button|向右滚动标签页|标签页溢出|
|DTabBarAddButton|button|新建标签页|有文档打开|
|Button_SelectFile|button|选择文件（空状态页）|无文档打开|
|Label_Icon|image|空状态图标|无文档打开|
|DTitlebarMainMenu|popup menu|主菜单弹出框|点击主菜单按钮|
|新窗口|menu item|打开新窗口|始终|
|新标签页|menu item|新建标签页|始终|
|保存|menu item|保存当前文档|有文档打开|
|另存为|menu item|另存当前文档|有文档打开|
|在文件管理器中显示|menu item|在文件管理器中定位文件|有文档打开|
|放大镜|menu item|放大镜模式|有文档打开|
|工具|menu item|工具子菜单（手形/选择）|有文档打开|
|选择工具|menu item|切换选择工具|主菜单→工具 或 右键菜单→手形工具子菜单|
|手形工具|menu item|切换手形工具|主菜单→工具 或 右键菜单→手形工具子菜单|
|搜索|menu item|打开搜索功能|始终|
|打印|menu item|打印文档|始终|
|主题|menu item|主题子菜单|始终|
|浅色|menu item|切换浅色主题|主菜单→主题 或 主题子菜单|
|深色|menu item|切换深色主题|主菜单→主题 或 主题子菜单|
|跟随系统|menu item|跟随系统主题|主菜单→主题 或 主题子菜单|
|帮助|menu item|打开帮助手册|始终|
|关于|menu item|查看关于信息|始终|
|退出|menu item|退出应用|始终|
|BrowserMenu|panel|右键菜单容器|右键点击文档区域|
|Menu_Hand|popup menu|手形工具子菜单（主菜单内）|主菜单→工具|
|DTitlebarThemeMenu|popup menu|主题子菜单（主菜单内）|主菜单→主题|
|SideBarImageListView|panel|侧栏缩略图列表视图|侧栏打开|
|EncryptionPage|panel|加密文档页面|打开加密文档|
|HandleMenu|panel|右键处理菜单（注释操作）|选中注释区域|
|PagingWidget|panel|翻页控件|有文档打开|
|TextEditShadowWidget|panel|文本编辑阴影控件|文本编辑模式|
|TransparentTextEdit|panel|透明文本编辑框|文本编辑模式|

---

## 2. 界面-元素映射

|界面|包含元素|
|----|--------|
|主窗口|Form_DMainWindow, Form_DMainWindowTitlebar, Form_DTitlebarLeftArea, Form_DTitlebarRightArea, Form_DTitlebarButtonArea, DTitlebarIconLabel, DTitlebarDWindowOptionButton, DTitlebarDWindowMinButton, DTitlebarDWindowMaxButton, DTitlebarDWindowQuitFullscreenButton, DTitlebarDWindowCloseButton, Form_TitleWidget, Button_ThumbnailToggle, Form_scaleEdit_P, DLineEditChildLineEdit, Form_Central, Form_widget_1|
|空状态页|Form_CentralNavPage, Button_SelectFile, Label_Icon|
|文档页|Form_CentralDocPage, Form_DocSheet, Form_SheetBrowser, Form_BrowserPage, Form_SheetSidebar, Form_FindWidget|
|标签栏|Form_DocTabBar, 向左滚动, 向右滚动, DTabBarAddButton|
|主菜单弹出框|DTitlebarMainMenu, 新窗口, 新标签页, 保存, 另存为, 在文件管理器中显示, 放大镜, 工具, Menu_Hand, 选择工具, 手形工具, 搜索, 打印, 主题, DTitlebarThemeMenu, 浅色, 深色, 跟随系统, 帮助, 关于, 退出|
|右键菜单|BrowserMenu|
|工具子菜单（主菜单内）|Menu_Hand, 选择工具, 手形工具|
|工具子菜单（右键菜单内）|Menu_Hand, 选择工具, 手形工具|
|主题子菜单（主菜单内）|DTitlebarThemeMenu, 浅色, 深色, 跟随系统|
|主题子菜单（独立）|DTitlebarThemeMenu, 浅色, 深色, 跟随系统|
|缩略图侧栏|SideBarImageListView, sideBarImageListView, sideBarImageListViewViewport|
|加密页面|EncryptionPage, ensureBtn, passwdEdit|
|注释处理菜单|HandleMenu, defaultshape, handleshape|
|翻页控件|PagingWidget, Edit_Page, Edit_Page_P, thumbnailNextBtn, thumbnailPreBtn|
|文本编辑控件|TextEditShadowWidget, TransparentTextEdit|

---

## 3. 功能-操作-断言映射

|功能|操作|断言目标|
|----|----|--------|
|应用启动|启动 deepin-reader 应用|Form_DMainWindow|
|应用启动（空状态）|启动后无文档加载|Form_CentralNavPage|
|打开文件-选择文件按钮|点击 Button_SelectFile 选择文件|Button_SelectFile|
|打开文件-拖拽|拖拽文件到主窗口|Form_CentralDocPage|
|打开文件-Ctrl+O|按下 Ctrl+O 打开文件选择对话框|Form_CentralDocPage|
|打开加密文档|输入密码打开加密文档|EncryptionPage|
|打开加密文档-确认按钮|点击加密页面确认按钮|ensureBtn|
|打开加密文档-密码输入框|点击加密页面密码框|passwdEdit|
|关闭窗口|点击关闭按钮|DTitlebarDWindowCloseButton|
|最小化窗口|点击最小化按钮|DTitlebarDWindowMinButton|
|最大化窗口|点击最大化按钮|DTitlebarDWindowMaxButton|
|还原窗口|已最大化时点击最大化按钮|DTitlebarDWindowMaxButton|
|退出全屏|全屏状态下点击退出全屏按钮|DTitlebarDWindowQuitFullscreenButton|
|全屏（按钮）|全屏状态下显示退出全屏按钮|DTitlebarDWindowQuitFullscreenButton|
|应用图标菜单|点击应用图标|DTitlebarIconLabel|
|打开主菜单|点击主菜单按钮|DTitlebarDWindowOptionButton|
|主菜单弹出|主菜单弹出后可见|DTitlebarMainMenu|
|新窗口|主菜单→新窗口|新窗口|
|新标签页|主菜单→新标签页|新标签页|
|保存文档|主菜单→保存|保存|
|另存为文档|主菜单→另存为|另存为|
|在文件管理器中显示|主菜单→在文件管理器中显示|在文件管理器中显示|
|放大镜模式|主菜单→放大镜|放大镜|
|打开工具子菜单|主菜单→工具|Menu_Hand|
|选择工具|主菜单→工具→选择工具|选择工具|
|手形工具|主菜单→工具→手形工具|手形工具|
|搜索功能|主菜单→搜索|搜索|
|打印文档|主菜单→打印|打印|
|打开主题子菜单|主菜单→主题|DTitlebarThemeMenu|
|浅色主题|主菜单→主题→浅色|浅色|
|深色主题|主菜单→主题→深色|深色|
|跟随系统主题|主菜单→主题→跟随系统|跟随系统|
|打开帮助|主菜单→帮助|帮助|
|关于信息|主菜单→关于|关于|
|退出应用|主菜单→退出|退出|
|右键菜单弹出|右键点击文档区域|BrowserMenu|
|右键菜单→手形工具子菜单|右键菜单→手形工具→选择工具/手形工具|Menu_Hand|
|右键菜单→选择工具|右键菜单→手形工具→选择工具|选择工具|
|右键菜单→手形工具|右键菜单→手形工具→手形工具|手形工具|
|缩略图开关|点击标题栏缩略图按钮|Button_ThumbnailToggle|
|侧栏缩略图列表|打开侧栏后显示缩略图列表|SideBarImageListView|
|标签页新建|点击新建标签按钮|DTabBarAddButton|
|标签页新建（主菜单）|主菜单→新标签页|新标签页|
|标签页向左滚动|标签页溢出时点击向左滚动|向左滚动|
|标签页向右滚动|标签页溢出时点击向右滚动|向右滚动|
|缩放比例输入|点击缩放比例输入框|DLineEditChildLineEdit|
|翻页-上一页|点击翻页控件的上一页按钮|thumbnailPreBtn|
|翻页-下一页|点击翻页控件的下一页按钮|thumbnailNextBtn|
|翻页-页码输入|点击翻页控件页码输入框|Edit_Page|
|翻页-总页数显示|翻页控件总页数显示|Edit_Page_P|
|注释-添加后处理菜单|右键点击注释区域|HandleMenu|
|注释-注释处理菜单|右键点击注释图标|HandleMenu|
|注释-默认形状|HandleMenu 弹出后默认形状可见|defaultshape|
|注释-处理形状|HandleMenu 弹出后处理形状可见|handleshape|
|文本编辑-编辑框|进入文本编辑模式|TextEditShadowWidget|
|文本编辑-透明编辑框|进入文本编辑模式|TransparentTextEdit|
|全屏（右键）|右键菜单→全屏后退出全屏按钮可见|DTitlebarDWindowQuitFullscreenButton|
|全屏（F11）|按下 F11 后退出全屏按钮可见|DTitlebarDWindowQuitFullscreenButton|
|Esc 退出全屏|全屏时按下 Esc 退出全屏|DTitlebarDWindowQuitFullscreenButton|
|主题切换-浅色|主菜单→主题→浅色|浅色|
|主题切换-深色|主菜单→主题→深色|深色|
|主题切换-跟随系统|主菜单→主题→跟随系统|跟随系统|
|主菜单-主题子菜单弹出|主菜单→主题|DTitlebarThemeMenu|
|主菜单-工具子菜单弹出|主菜单→工具|Menu_Hand|
|多标签页左右滚动|标签页溢出时滚动按钮可见|向左滚动|
|多标签页右滚动|标签页溢出时滚动按钮可见|向右滚动|