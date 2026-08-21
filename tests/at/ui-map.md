# deepin-reader UI 图谱

## 组件树 (Mermaid)

```mermaid
graph TD
    subgraph "MainWindow [DMainWindow]"
        TitleBar["DTitlebar<br/>titlebar()"]
        TitleBar --> TitleWidget["TitleWidget [BaseWidget]"]
        TitleBar --> DocTabLabel["DLabel (m_tabLabel)"]
        TitleBar --> OptBtn["DIconButton (DTitlebarDWindowOptionButton)"]
        TitleBar --> QuitFullBtn["DWindowQuitFullButton"]
        TitleBar --> MinBtn["DIconButton (DTitlebarDWindowMinButton)"]
        TitleBar --> MaxBtn["DIconButton (DTitlebarDWindowMaxButton)"]
        TitleBar --> CloseBtn["DIconButton (DTitlebarDWindowCloseButton)"]

        TitleWidget --> ThumbnailToggle["DIconButton (Thumbnails)<br/>AccessibleName: Button_ThumbnailToggle"]
        TitleWidget --> ScaleWidget["ScaleWidget [DWidget]"]
        ScaleWidget --> ZoomOut["DIconButton (SP_DecreaseElement)"]
        ScaleWidget --> ScaleEdit["DLineEdit (scaleEdit)"]
        ScaleWidget --> ZoomIn["DIconButton (SP_IncreaseElement)"]
        ScaleWidget --> ArrowBtn["DIconButton (editArrowBtn)"]

        Central["Central [BaseWidget]"]
        Central -->|QStackedLayout[0]| NavPage["CentralNavPage [BaseWidget]"]
        Central -->|QStackedLayout[1]| DocPage["CentralDocPage [BaseWidget]"]
        Central --> RestoreTip["RestoreTipWidget [DWidget]"]

        NavPage --> IconLabel["DLabel (iconSvg) AccessibleName: Label_Icon"]
        NavPage --> DragHint["DLabel AccessibleName: Label_Drag documents here"]
        NavPage --> FormatLabel["DLabel AccessibleName: Label_format supported: ..."]
        NavPage --> SelectFileBtn["DSuggestButton (SelectFileBtn) AccessibleName: SelectFile"]

        DocPage --> TabBar["DocTabBar [DTabBar]"]
        DocPage -->|QStackedLayout| DocSheets["DocSheet(s) [DSplitter]"]

        DocSheets --> Sidebar["SheetSidebar [BaseWidget]"]
        DocSheets --> Browser["SheetBrowser [DGraphicsView]"]

        Sidebar -->|QStackedLayout| ThumbnailWidget["ThumbnailWidget [BaseWidget]"]
        Sidebar -->|QStackedLayout| CatalogWidget["CatalogWidget [BaseWidget]"]
        Sidebar -->|QStackedLayout| BookMarkWidget["BookMarkWidget [BaseWidget]"]
        Sidebar -->|QStackedLayout| NotesWidget["NotesWidget [BaseWidget]"]
        Sidebar -->|QStackedLayout| SearchWidget["SearchResWidget [BaseWidget]"]
        Sidebar --> SidebarBtnGroup["DToolButtons (thumbnail/catalog/bookmark/annotation/search)"]
        SidebarBtnGroup --> BtnThumb["DToolButton (thumbnail) AccessibleName: Button_thumbnail"]
        SidebarBtnGroup --> BtnCatalog["DToolButton (catalog) AccessibleName: Button_catalog"]
        SidebarBtnGroup --> BtnBookmark["DToolButton (bookmark) AccessibleName: Button_bookmark"]
        SidebarBtnGroup --> BtnAnnotation["DToolButton (annotation) AccessibleName: Button_annotation"]
        SidebarBtnGroup --> BtnSearch["DToolButton (search) AccessibleName: Button_search"]

        ThumbnailWidget --> SidebarThumbList["SideBarImageListView [DListView]"]
        ThumbnailWidget --> PagingWidget["PagingWidget [BaseWidget]"]

        PagingWidget --> PageEdit["DLineEdit (Edit_Page) AccessibleName: Page"]
        PagingWidget --> PagePrev["DIconButton (thumbnailPreBtn) AccessibleName: Button_ThumbnailPre"]
        PagingWidget --> PageNext["DIconButton (thumbnailNextBtn) AccessibleName: Button_ThumbnailNext"]
        PagingWidget --> TotalPages["DLabel AccessibleName: Label_TotalPage"]
        PagingWidget --> CurrentPage["DLabel AccessibleName: CurrentPage"]

        CatalogWidget --> CatalogTree["CatalogTreeView [DTreeView]"]
        BookMarkWidget --> BookmarkList["SideBarImageListView [DListView]"]
        NotesWidget --> NotesList["SideBarImageListView [DListView]"]
        SearchWidget --> SearchList["SideBarImageListView [DListView]"]
    end

    subgraph "Overlay / Floating"
        BrowserMenu["BrowserMenu [DMenu] AccessibleName: Menu_Browser"]
        TitleMenu["TitleMenu [DMenu]"]
        HandleMenu["HandleMenu [DMenu] AccessibleName: Menu_Hand"]
        ScaleMenu["ScaleMenu [DMenu]"]
        NoteMenu["DMenu (m_pNoteMenu) AccessibleName: Menu_Note"]
        BookMarkMenu["DMenu (m_pBookMarkMenu) AccessibleName: Menu_BookMark"]
        FindWidget["FindWidget [DFloatingWidget]"]
        EncryptionPage["EncryptionPage [DWidget]"]
        TipsWidget["TipsWidget [DWidget]"]
        SlideWidget["SlideWidget [DWidget]"]
        SlidePlayWidget["SlidePlayWidget [DFloatingWidget]"]
        ProgressDialog["ProgressDialog [DDialog]"]
        SecurityDialog["SecurityDialog [DDialog]"]
        FileAttrWidget["FileAttrWidget [DAbstractDialog]"]
        AttrScrollWidget["AttrScrollWidget [DFrame]"]
        EyeProtectionAction["EyeProtectionAction [QWidgetAction]"]
        ColorWidgetAction["ColorWidgetAction [QWidgetAction]"]
        TextEditWidget["TextEditWidget [BaseWidget]"]
        TextEditShadowWidget["TextEditShadowWidget [DWidget]"]
        TransparentTextEdit["TransparentTextEdit"]

        FindWidget --> SearchEdit["DSearchEdit (findSearchEdit_P) AccessibleName: Form_findSearchEdit_P"]
        FindWidget --> FindPrev["DIconButton (SP_ArrowUpBtn)"]
        FindWidget --> FindNext["DIconButton (SP_ArrowDownBtn)"]
        FindWidget --> FindClose["DDialogCloseButton (closeButton)"]

        EncryptionPage --> EncryptPassword["DPasswordEdit"]
        EncryptionPage --> EncryptOK["DPushButton (ensureBtn)"]
    end

    subgraph "Document Area"
        Browser --> BrowserPage["BrowserPage(s) [QGraphicsItem]"]
        Browser --> BrowserAnnotation["BrowserAnnotation [QGraphicsItem]"]
        Browser --> BrowserMagnifier["BrowserMagnifier [QLabel]"]
        Browser --> BrowserWord["BrowserWord [QGraphicsItem]"]
    end
```

## 控件表

| 控件类 | 基类 | 文件 | 行 | AT 注册 | 显式名 | 说明 |
|--------|------|------|----|---------|--------|------|
| MainWindow | DMainWindow | reader/MainWindow.h:14 | 14-193 | SET_FORM_ACCESSIBLE("MainWindow") | ✗ | 无 setAccessibleName，但 AT 注册固定名 "MainWindow" |
| Central | BaseWidget → DWidget | reader/uiframe/Central.h:23 | 23-184 | SET_FORM_ACCESSIBLE("Central") | ✗ | 同上 |
| CentralDocPage | BaseWidget → DWidget | reader/uiframe/CentralDocPage.h:28 | 28-380 | SET_FORM_ACCESSIBLE("CentralDocPage") | ✗ | 同上 |
| CentralNavPage | BaseWidget → DWidget | reader/uiframe/CentralNavPage.h:15 | 15-37 | SET_FORM_ACCESSIBLE("CentralNavPage") | ✓ | 子控件有显式名 |
| DocSheet | DSplitter | reader/uiframe/DocSheet.h:53 | 53-931 | SET_FORM_ACCESSIBLE("DocSheet") | ✗ | 同上 |
| DocTabBar | DTabBar | reader/uiframe/DocTabBar.h:22 | 22-207 | SET_FORM_ACCESSIBLE("DocTabBar") | ✗ | 同上 |
| TitleWidget | BaseWidget → DWidget | reader/uiframe/TitleWidget.h:24 | 24-79 | SET_FORM_ACCESSIBLE("TitleWidget") | ✓ | 子控件有显式名 |
| SheetSidebar | BaseWidget → DWidget | reader/sidebar/SheetSidebar.h:42 | 42-246 | SET_FORM_ACCESSIBLE("SheetSidebar") | ✓ | createBtn 设显式名 |
| SheetBrowser | DGraphicsView | reader/browser/SheetBrowser.h:52 | 52-745 | SET_FORM_ACCESSIBLE("SheetBrowser") | ✓ | 构造函数内设 |
| ThumbnailWidget | BaseWidget → DWidget | reader/sidebar/ThumbnailWidget.h:22 | 22-115 | SET_FORM_ACCESSIBLE("ThumbnailWidget") | ✓ | initWidget 内设 |
| CatalogWidget | BaseWidget → DWidget | reader/sidebar/CatalogWidget.h:20 | 20-106 | SET_FORM_ACCESSIBLE("CatalogWidget") | ✓ | initWidget 内设 |
| BookMarkWidget | BaseWidget → DWidget | reader/sidebar/BookMarkWidget.h:20 | 20-137 | SET_FORM_ACCESSIBLE("BookMarkWidget") | ✓ | initWidget 内设 |
| NotesWidget | BaseWidget → DWidget | reader/sidebar/NotesWidget.h:24 | 24-162 | SET_FORM_ACCESSIBLE("NotesWidget") | ✓ | initWidget 内设 |
| SearchResWidget | BaseWidget → DWidget | reader/sidebar/SearchResWidget.h:22 | 22-86 | SET_FORM_ACCESSIBLE("SearchResWidget") | ✓ | initWidget 内设 |
| SideBarImageListView | DListView | reader/sidebar/SideBarImageListview.h:29 | 29-192 | ✗ | ✓ | 构造函数有 setObjectName |
| CatalogTreeView | DTreeView | reader/sidebar/CatalogTreeView.h:24 | 24-187 | ✗ | ✗ | 无 AT 注册，无显式名 |
| PagingWidget | BaseWidget → DWidget | reader/widgets/PagingWidget.h:22 | 22-131 | ✗ | ✓ | 子控件有显式名 |
| FindWidget | DFloatingWidget | reader/widgets/FindWidget.h:25 | 25-129 | ✗ | ✓ | 子控件有显式名 |
| ScaleWidget | DWidget | reader/widgets/ScaleWidget.h:25 | 25-109 | ✗ | ✓ | 子控件有 objectName |
| TitleMenu | DMenu | reader/uiframe/TitleMenu.h:20 | 20-84 | ✗ | ✓ | HandleMenu 设 "Menu_Hand" |
| HandleMenu | DMenu | reader/widgets/HandleMenu.h:17 | 17-56 | ✗ | ✗ | 无显式名 |
| ScaleMenu | DMenu | reader/widgets/ScaleMenu.h:17 | 17-86 | ✗ | ✗ | 无显式名 |
| BrowserMenu | DMenu | reader/browser/BrowserMenu.h:27 | 27-83 | ✗ | ✓ | setAccessibleName("Menu_Browser") |
| EncryptionPage | DWidget | reader/widgets/EncryptionPage.h:17 | 17-24 | ✗ | ✓ | 子控件有 objectName |
| SecurityDialog | DDialog | reader/widgets/SecurityDialog.h:21 | 21-49 | ✗ | ✗ | 无显式名 |
| ProgressDialog | DDialog | reader/widgets/ProgressDialog.h:11 | 11-19 | ✗ | ✗ | 无显式名 |
| FileAttrWidget | DAbstractDialog | reader/widgets/FileAttrWidget.h:22 | 22-73 | ✗ | ✗ | 无显式名 |
| RestoreTipWidget | DWidget | reader/widgets/RestoreTipWidget.h:23 | 23-90 | ✗ | ✗ | 无显式名 |
| TipsWidget | DWidget | reader/widgets/TipsWidget.h:15 | 15-21 | ✗ | ✗ | 无显式名 |
| SlideWidget | DWidget | reader/widgets/SlideWidget.h:23 | 23-180 | ✗ | ✗ | 无显式名 |
| SlidePlayWidget | DFloatingWidget | reader/widgets/SlidePlayWidget.h:21 | 21-168 | ✗ | ✗ | 无显式名 |
| BrowserPage | QGraphicsItem | reader/browser/BrowserPage.h:33 | 33-438 | ✗ | ✗ | 非 QWidget |
| BrowserMagniFier | QLabel | reader/browser/BrowserMagniFier.h:58 | 58-101 | ✗ | ✗ | 无显式名 |
| LoadingWidget | QWidget | reader/uiframe/DocSheet.h:910 | 910-930 | ✗ | ✗ | 无显式名 |
| TextEditWidget | BaseWidget → DWidget | reader/widgets/TextEditWidget.h:23 | 23-146 | ✗ | ✗ | 无显式名 |
| TextEditShadowWidget | DWidget | reader/widgets/TextEditWidget.h:148 | 148-174 | ✓ | setObjectName | 构造函数内设 |
| TransparentTextEdit | | reader/widgets/TransparentTextEdit.cpp:22 | 22-51 | ✗ | ✓ | 构造函数内设 |

## 菜单索引

### 标题栏菜单 (TitleMenu)
| 菜单项 | ObjectName | 触发条件 | 位置 |
|--------|-----------|---------|------|
| New window | "New window" | 始终可用 | TitleMenu.cpp:17 |
| New tab | "New tab" | 始终可用 | TitleMenu.cpp:17 |
| Save | "Save" | 文档有未保存更改 | TitleMenu.cpp:27 |
| Save as | "Save as" | 始终可用 | TitleMenu.cpp:27 |
| Display in file manager | "Display in file manager" | 始终可用 | TitleMenu.cpp:32 |
| Magnifer | "Magnifer" | 始终可用 | TitleMenu.cpp:32 |
| 护眼模式 | EyeProtectionAction | 始终可用 | TitleMenu.cpp:37 |
| 工具 (HandleMenu) | "Menu_Hand" | 文档打开后可用 | TitleMenu.cpp:40-44 |
| ├─ Select Text | "defaultshape" | 文档打开后 | HandleMenu.cpp:25-31 |
| └─ Hand Tool | "handleshape" | 文档打开后 | HandleMenu.cpp:33-39 |
| Search | "Search" | PDF/DOCX/XPS 文档 | TitleMenu.cpp:46 |
| Print | "Print" | 始终可用 | TitleMenu.cpp:46 |

### 文档右键菜单 (BrowserMenu)
| 菜单项 | ObjectName | 菜单类型 | 触发条件 |
|--------|-----------|---------|---------|
| Search | "Search" | DOC_MENU_DEFAULT / DOC_MENU_KEY | PDF/DOCX/XPS |
| Add bookmark | "AddBookmark" | 所有 | 无书签 |
| Remove bookmark | "RemoveBookmark" | 所有 | 已有书签 |
| Add annotation | "AddAnnotationIcon" | DOC_MENU_DEFAULT | PDF/DOCX |
| Fullscreen | "Fullscreen" | DOC_MENU_DEFAULT / DOC_MENU_KEY | 非全屏 |
| Exit fullscreen | "ExitFullscreen" | DOC_MENU_DEFAULT / DOC_MENU_KEY | 全屏 |
| Slide show | "SlideShow" | 所有 | 始终 |
| First page | "FirstPage" | DOC_MENU_DEFAULT / DOC_MENU_KEY | 非首页 |
| Previous page | "PreviousPage" | DOC_MENU_DEFAULT / DOC_MENU_KEY | 非首页 |
| Next page | "NextPage" | DOC_MENU_DEFAULT / DOC_MENU_KEY | 非末页 |
| Last page | "LastPage" | DOC_MENU_DEFAULT / DOC_MENU_KEY | 非末页 |
| Rotate left | "RotateLeft" | 所有 | 始终 |
| Rotate right | "RotateRight" | 所有 | 始终 |
| Print | "Print" | 所有 | 始终 |
| Document info | "DocumentInfo" | 所有 | 始终 |
| Copy | "Copy" | DOC_MENU_SELECT_TEXT | 文本选中 |
| Copy | "CopyAnnoText" | DOC_MENU_ANNO_ICON / DOC_MENU_ANNO_HIGHLIGHT | 注释选中 |
| Highlight | "AddTextHighlight" | DOC_MENU_SELECT_TEXT | PDF/DOCX |
| Change color | "ChangeAnnotationColor" | DOC_MENU_ANNO_HIGHLIGHT | PDF/DOCX |
| Remove highlight | "RemoveHighlight" | DOC_MENU_ANNO_HIGHLIGHT | PDF/DOCX |
| Remove annotation | "RemoveAnnotation" | DOC_MENU_ANNO_ICON | PDF/DOCX |
| Add annotation | "AddAnnotationHighlight" | DOC_MENU_ANNO_HIGHLIGHT / DOC_MENU_SELECT_TEXT | PDF/DOCX |

### 侧栏注释菜单 (NoteMenu)
| 菜单项 | 事件 | 位置 |
|--------|------|------|
| Copy | E_NOTE_COPY | SideBarImageListview.cpp:278 |
| Remove annotation | E_NOTE_DELETE | SideBarImageListview.cpp:284 |
| Remove all | E_NOTE_DELETE_ALL | SideBarImageListview.cpp:290 |

### 侧栏书签菜单 (BookMarkMenu)
| 菜单项 | 事件 | 位置 |
|--------|------|------|
| Remove bookmark | E_BOOKMARK_DELETE | SideBarImageListview.cpp:306 |
| Remove all | E_BOOKMARK_DELETE_ALL | SideBarImageListview.cpp:311 |

### 缩放菜单 (ScaleMenu)
| 菜单项 | 槽函数 | 位置 |
|--------|--------|------|
| 双页显示 | onTwoPage | ScaleMenu.h:36 |
| 适应高度 | onFiteH | ScaleMenu.h:39 |
| 适应宽度 | onFiteW | ScaleMenu.h:42 |
| 默认大小 | onDefaultPage | ScaleMenu.h:45 |
| 适合页面 | onFitPage | ScaleMenu.h:48 |
| 百分比 | onScaleFactor | ScaleMenu.h:51 |

## 对话框索引

| 对话框 | 类型 | 创建位置 | 说明 |
|--------|------|---------|------|
| DFileDialog | DFileDialog | Central.cpp:148 | 打开文件选择 |
| DPrintPreviewDialog | DPrintPreviewDialog | DocSheet.cpp (onPopPrintDialog) | 打印预览 |
| SaveDialog (退出确认) | DDialog(DTipDialog) | SaveDialog::showExitDialog | 退出保存提示 |
| SaveDialog (提示) | DDialog | SaveDialog::showTipDialog | 通用提示 |
| SecurityDialog | DDialog | SecurityDialog.cpp | 安全提示对话框 |
| ProgressDialog | DDialog | ProgressDialog.h | 进度对话框 |
| FileAttrWidget | DAbstractDialog | FileAttrWidget.h | 文件属性 |
| ShortCutShow | 使用 QObject + DDialog 内部 | Central.cpp:222 | 快捷键预览 |
| EncryptionPage | DWidget (嵌入显示) | DocSheet.cpp:1693 | 密码输入页 |

## 快捷键索引

| 快捷键 | 功能 | 位置 |
|--------|------|------|
| Ctrl+F | 查找 | Central.cpp:57 |
| Ctrl+O | 打开文件 | Central.cpp:58 |
| Ctrl+P | 打印 | Central.cpp:59 |
| Ctrl+S | 保存 | Central.cpp:60 |
| Ctrl+C | 复制 | Central.cpp:61 |
| Left | 上一页/左滚 | Central.cpp:64 |
| Right | 下一页/右滚 | Central.cpp:65 |
| Up | 上滚 | Central.cpp:67 |
| Down | 下滚 | Central.cpp:68 |
| Space | 翻页 | Central.cpp:69 |
| Esc | 退出全屏/关闭查找 | Central.cpp:70 |
| F5 | 幻灯片放映 | Central.cpp:71 |
| F11 | 全屏切换 | Central.cpp:72 |
| Alt+1 | 侧栏缩略图 | Central.cpp:73 |
| Alt+2 | 侧栏目录 | Central.cpp:74 |
| Alt+A | 添加注释 | Central.cpp:75 |
| Alt+H | 手型工具 | Central.cpp:76 |
| Alt+Z | 缩放工具 | Central.cpp:77 |
| Ctrl+1 | 适应宽度 | Central.cpp:78 |
| Ctrl+2 | 适应高度 | Central.cpp:79 |
| Ctrl+3 | 双页显示 | Central.cpp:80 |
| Ctrl+D | 添加书签 | Central.cpp:81 |
| Ctrl+M | 护眼模式 | Central.cpp:82 |
| Ctrl+R | 旋转 | Central.cpp:83 |
| Ctrl+- | 缩小 | Central.cpp:84 |
| Ctrl+= | 放大 | Central.cpp:85 |
| Ctrl++ | 放大(备选) | Central.cpp:86 |
| Ctrl+Shift+R | 逆时针旋转 | Central.cpp:87 |
| Ctrl+Shift+S | 另存为 | Central.cpp:88 |
| Ctrl+Shift+/ | 快捷键预览 | Central.cpp:89 |
| Ctrl+Home | 首页 | Central.cpp:90 |
| Ctrl+End | 末页 | Central.cpp:91 |