# Expected AT-SPI Elements

## 命名规则
AT-SPI 名称由 `getAccessibleName()` 在 `accessibledefine.h` 中生成：
- 格式：`<RolePrefix>_<accessibleName|objectName|fallback>`
- RolePrefix 映射：Form→`Form`, Button→`Button`, Label→`Label`, Editable→`Editable`, Slider→`Slider`
- 同名控件自动加 `_N` 后缀去重
- 优先顺序：`setAccessibleName()` > `setObjectName()` > 宏模板中的 fallback

## 预期元素清单

### 主窗口
| 预期 AT-SPI 名称 | Role | 源码推导 | 可见条件 | 交互性 |
|-----------------|------|---------|---------|-------|
| `Form_MainWindow` | Form | SET_FORM_ACCESSIBLE(MainWindow) | 始终 | 否(容器) |
| `Form_Central` | Form | SET_FORM_ACCESSIBLE(Central) | 始终 | 否(容器) |
| `Form_CentralNavPage` | Form | SET_FORM_ACCESSIBLE(CentralNavPage) | 无文档时 | 否(容器) |
| `Form_CentralDocPage` | Form | SET_FORM_ACCESSIBLE(CentralDocPage) | 有文档时 | 否(容器) |
| `Form_DocSheet` | Form | SET_FORM_ACCESSIBLE(DocSheet) | 有文档时 | 否(容器) |
| `Form_DocTabBar` | Form | SET_FORM_ACCESSIBLE(DocTabBar) | 有文档时 | 是 |
| `Form_TitleWidget` | Form | SET_FORM_ACCESSIBLE(TitleWidget) | 始终 | 否(容器) |

### 导航页(无文档时)
| 预期 AT-SPI 名称 | Role | 源码推导 | 可见条件 | 交互性 |
|-----------------|------|---------|---------|-------|
| `Label_Icon` | Label | setAccessibleName("Label_Icon") | 无文档 | 否 |
| `Label_Drag documents here` | Label | setAccessibleName("Label_Drag documents here") | 无文档 | 否 |
| `Label_format supported: PDF,DJVU,DOCX` | Label | setAccessibleName("Label_format supported: PDF%1").arg(supportedFormats) | 无文档 | 否 |
| `SelectFile` | Button | setAccessibleName("SelectFile") | 无文档 | 是(按钮) |

### 标题栏
| 预期 AT-SPI 名称 | Role | 源码推导 | 可见条件 | 交互性 |
|-----------------|------|---------|---------|-------|
| `Button_ThumbnailToggle` | Button | setAccessibleName("Button_ThumbnailToggle") | 始终 | 是(切换按钮) |
| `Form_TitleWidget` | Form | SET_FORM_ACCESSIBLE(TitleWidget) | 始终 | 否(容器) |
| `Menu_Title` | Form | setAccessibleName("Menu_Title") | 点击主菜单按钮时可见 | 否(菜单弹出) |
| `Menu_Hand` | Form | setAccessibleName("Menu_Hand") | 主菜单打开时可见 | 否(子菜单弹出) |
| `DTitlebar` | Form | SET_FORM_ACCESSIBLE(DTitlebar) | 始终 | 是(标题栏) |

### 标题栏缩放控件(ScaleWidget)
| 预期 AT-SPI 名称 | Role | 源码推导 | 可见条件 | 交互性 |
|-----------------|------|---------|---------|-------|
| `scaleEdit` | Editable | objectName="scaleEdit" | 始终 | 是(输入框) |
| `SP_DecreaseElement` | Button | objectName="SP_DecreaseElement" | 始终 | 是(缩小按钮) |
| `SP_IncreaseElement` | Button | objectName="SP_IncreaseElement" | 始终 | 是(放大按钮) |
| `editArrowBtn` | Button | objectName="editArrowBtn" | 始终 | 是(下拉选择按钮) |

### 查找栏(FindWidget)
| 预期 AT-SPI 名称 | Role | 源码推导 | 可见条件 | 交互性 |
|-----------------|------|---------|---------|-------|
| `Form_findSearchEdit_P` | Form | setAccessibleName("Form_findSearchEdit_P") | 查找栏打开时 | 否(容器) |
| `DLineEditChildLineEdit` | Editable | setAccessibleName("DLineEditChildLineEdit") | 查找栏打开时 | 是(输入框) |
| `SP_ArrowUpBtn` | Button | objectName="SP_ArrowUpBtn" | 查找栏打开时 | 是(前一个按钮) |
| `SP_ArrowDownBtn` | Button | objectName="SP_ArrowDownBtn" | 查找栏打开时 | 是(后一个按钮) |
| `closeButton` | Button | objectName="closeButton" | 查找栏打开时 | 是(关闭按钮) |

### 翻页栏(PagingWidget)
| 预期 AT-SPI 名称 | Role | 源码推导 | 可见条件 | 交互性 |
|-----------------|------|---------|---------|-------|
| `Page` | Editable | setAccessibleName("Page") | 文档打开时 | 是(跳转页输入) |
| `pageEdit` | Editable | setAccessibleName("pageEdit") | 文档打开时 | 是(子输入框) |
| `Label_TotalPage` | Label | setAccessibleName("Label_TotalPage") | 文档打开时 | 否 |
| `CurrentPage` | Label | setAccessibleName("CurrentPage") | 文档打开时，较多页时 | 否 |
| `Button_ThumbnailPre` | Button | setAccessibleName("Button_ThumbnailPre") | 文档打开时 | 是(上一页按钮) |
| `Button_ThumbnailNext` | Button | setAccessibleName("Button_ThumbnailNext") | 文档打开时 | 是(下一页按钮) |

### 侧栏切换按钮(SheetSidebar)
| 预期 AT-SPI 名称 | Role | 源码推导 | 可见条件 | 交互性 |
|-----------------|------|---------|---------|-------|
| `Button_thumbnail` | Button | setAccessibleName("Button_thumbnail") | 文档打开时 | 是(切换按钮) |
| `Button_catalog` | Button | setAccessibleName("Button_catalog") | 文档打开时(PDF/DOCX/XPS) | 是(切换按钮) |
| `Button_bookmark` | Button | setAccessibleName("Button_bookmark") | 文档打开时 | 是(切换按钮) |
| `Button_annotation` | Button | setAccessibleName("Button_annotation") | 文档打开时(PDF/DOCX) | 是(切换按钮) |
| `Button_search` | Button | setAccessibleName("Button_search") | 文档打开时(搜索有结果) | 是(切换按钮) |

### 侧栏面板
| 预期 AT-SPI 名称 | Role | 源码推导 | 可见条件 | 交互性 |
|-----------------|------|---------|---------|-------|
| `Form_SheetSidebar` | Form | SET_FORM_ACCESSIBLE(SheetSidebar) | 文档打开时 | 否(容器) |
| `Form_ThumbnailWidget` | Form | SET_FORM_ACCESSIBLE(ThumbnailWidget) | 侧栏缩略图选中时 | 是(可点击缩略图) |
| `Form_CatalogWidget` | Form | SET_FORM_ACCESSIBLE(CatalogWidget) | 侧栏目录选中时 | 是(可点击目录项) |
| `Form_BookMarkWidget` | Form | SET_FORM_ACCESSIBLE(BookMarkWidget) | 侧栏书签选中时 | 是(可点击书签项) |
| `Form_NotesWidget` | Form | SET_FORM_ACCESSIBLE(NotesWidget) | 侧栏注释选中时 | 是(可点击注释项) |
| `Form_SearchResWidget` | Form | SET_FORM_ACCESSIBLE(SearchResWidget) | 搜索有结果时 | 是(可点击搜索结果) |

### 文档浏览区域
| 预期 AT-SPI 名称 | Role | 源码推导 | 可见条件 | 交互性 |
|-----------------|------|---------|---------|-------|
| `Form_SheetBrowser` | Form | SET_FORM_ACCESSIBLE(SheetBrowser) | 文档打开时 | 否(容器) |

### 主菜单项(通过DTK菜单操作)
| 预期 objectName | Role | 源码推导 | 可见条件 | 交互性 |
|----------------|------|---------|---------|-------|
| New window | Button | setObjectName("New window") | 始终 | 是(菜单项) |
| New tab | Button | setObjectName("New tab") | 始终 | 是(菜单项) |
| Save | Button | setObjectName("Save") | 文档打开 | 是(菜单项) |
| Save as | Button | setObjectName("Save as") | 文档打开 | 是(菜单项) |
| Display in file manager | Button | setObjectName("Display in file manager") | 文档打开 | 是(菜单项) |
| Magnifer | Button | setObjectName("Magnifer") | 文档打开 | 是(菜单项) |
| Search | Button | setObjectName("Search") | 文档打开 | 是(菜单项) |
| Print | Button | setObjectName("Print") | 文档打开 | 是(菜单项) |
| Select Text (Tools子菜单) | Button | setObjectName("defaultshape") | 文档打开 | 是(菜单项) |
| Hand Tool (Tools子菜单) | Button | setObjectName("handleshape") | 文档打开 | 是(菜单项) |

### 对话框
| 预期 AT-SPI 名称 | Role | 源码推导 | 可见条件 | 交互性 |
|-----------------|------|---------|---------|-------|
| `DDialog` | Form | SET_FORM_ACCESSIBLE(DDialog) | 对话框打开时 | 否(容器) |
| `DFileDialog` | Form | SET_FORM_ACCESSIBLE(DFileDialog) | 文件对话框打开时 | 是(文件选择) |

### 状态未覆盖的控件(需运行时验证)
| 预期名称 | 原因 |
|---------|------|
| EyeProtectionAction(Eye Protection) | 无 setAccessibleName/objectName，fallback 为当前选择值 |
| 侧栏各 widget 内部子控件(目录树、书签列表等) | 需运行时 dump 确定子级 AT-SPI 名称 |
| DocTabBar 标签页 | 标签页名称为文档文件名，运行时确定 |
| 缩放下拉菜单(ScaleMenu) | 弹出菜单，运行时确定 |
| 幻灯片播放按钮(SlidePlayWidget) | 弹出面板，运行时确定 |
| 恢复提示条(RestoreTipWidget) | 弹出后自动消失，运行时确定 |