# AT-SPI Implementation Checklist

## 覆盖统计

| 统计项 | 数值 |
|--------|------|
| 源码可确认的交互控件总数 | ~30 |
| 已有显式 setAccessibleName | ~25 |
| 已有 setObjectName(fallback) | ~5 |
| 通过 QAccessible installFactory 注册 | 已注册(accessible.h) |
| 缺口总数 | 5 |

## 无需修改(已有完整 AT-SPI 支持)

| 控件 | 文件 | AT-SPI 名称 | 注册方式 |
|------|------|------------|---------|
| MainWindow | reader/main.cpp | Form_MainWindow | installFactory + SET_FORM_ACCESSIBLE |
| Central | reader/uiframe/Central.h | Form_Central | installFactory + SET_FORM_ACCESSIBLE |
| CentralDocPage | reader/uiframe/CentralDocPage.h | Form_CentralDocPage | installFactory + SET_FORM_ACCESSIBLE |
| CentralNavPage | reader/uiframe/CentralNavPage.h | Form_CentralNavPage | installFactory + SET_FORM_ACCESSIBLE |
| DocSheet | reader/uiframe/DocSheet.h | Form_DocSheet | installFactory + SET_FORM_ACCESSIBLE |
| DocTabBar | reader/uiframe/DocTabBar.h | Form_DocTabBar | installFactory + SET_FORM_ACCESSIBLE |
| TitleWidget | reader/uiframe/TitleWidget.h | Form_TitleWidget | installFactory + SET_FORM_ACCESSIBLE |
| SheetSidebar | reader/sidebar/SheetSidebar.h | Form_SheetSidebar | installFactory + SET_FORM_ACCESSIBLE |
| SheetBrowser | reader/sidebar/SheetBrowser.h | Form_SheetBrowser | installFactory + SET_FORM_ACCESSIBLE |
| ThumbnailWidget | reader/sidebar/ThumbnailWidget.h | Form_ThumbnailWidget | installFactory + SET_FORM_ACCESSIBLE |
| CatalogWidget | reader/sidebar/CatalogWidget.h | Form_CatalogWidget | installFactory + SET_FORM_ACCESSIBLE |
| BookMarkWidget | reader/sidebar/BookMarkWidget.h | Form_BookMarkWidget | installFactory + SET_FORM_ACCESSIBLE |
| NotesWidget | reader/sidebar/NotesWidget.h | Form_NotesWidget | installFactory + SET_FORM_ACCESSIBLE |
| SearchResWidget | reader/sidebar/SearchResWidget.h | Form_SearchResWidget | installFactory + SET_FORM_ACCESSIBLE |

## 缺口清单

### GAP-01: 侧栏缩略图的内部列表项(SideBarImageListView)
- **优先级**: 高
- **目标名称**: `ThumbnailWgt_<index>` 或 `SideBarImageListView`
- **插入位置**: `reader/sidebar/SideBarImageListview.cpp` 构造函数末尾
- **当前状态**: QFrame 子级的 fallback 名称，无唯一标识
- **修改模板**:
```cpp
// 在 SideBarImageListView 构造函数末尾添加
this->setAccessibleName("SideBarImageListView");
```
- **验证断言**: AT 用例中 `assert_element` 检查 name=`SideBarImageListView`

### GAP-02: 目录树视图(CatalogTreeView)
- **优先级**: 高
- **目标名称**: `CatalogTreeView`
- **插入位置**: `reader/sidebar/CatalogTreeView.cpp` 构造函数
- **当前状态**: 无显式 AccessibleName，QTreeView 派生
- **修改模板**:
```cpp
this->setAccessibleName("CatalogTreeView");
```
- **验证断言**: `assert_element` 检查 name=`CatalogTreeView`

### GAP-03: 书签列表(BookMarkWidget 内部列表)
- **优先级**: 中
- **目标名称**: `BookMarkList`
- **插入位置**: `reader/sidebar/BookMarkWidget.cpp` initWidget 末尾
- **当前状态**: 内部 QListView 无显式名称
- **修改模板**:
```cpp
// 在 BookMarkWidget::initWidget 末尾
m_pListView->setAccessibleName("BookMarkList");
```
- **验证断言**: `assert_element` 检查 name=`BookMarkList`

### GAP-04: 注释列表(NotesWidget 内部列表)
- **优先级**: 中
- **目标名称**: `NotesList`
- **插入位置**: `reader/sidebar/NotesWidget.cpp` initWidget 末尾
- **当前状态**: 内部 QListView 无显式名称
- **修改模板**:
```cpp
// 在 NotesWidget::initWidget 末尾
m_pListView->setAccessibleName("NotesList");
```
- **验证断言**: `assert_element` 检查 name=`NotesList`

### GAP-05: 幻灯片播放控件(SlidePlayWidget)
- **优先级**: 低
- **目标名称**: `SlidePlayWidget`
- **插入位置**: `reader/widgets/SlidePlayWidget.cpp` 构造函数末尾
- **当前状态**: 按钮无显式 AccessibleName，只有 objectName
- **修改模板**:
```cpp
// 在 SlidePlayWidget 构造函数末尾
this->setAccessibleName("SlidePlayWidget");
```
- **验证断言**: `assert_element` 检查 name=`SlidePlayWidget`

## 动态风险说明

以下控件在运行时具有动态名称，AT 用例需要使用 `child_index` 或父级定位：

| 控件 | 动态因素 | 推荐 AT 定位策略 |
|------|---------|----------------|
| DocTabBar 标签页 | 标签名=文档文件名 | 通过 `DocTabBar` Form 容器 + `child_index` |
| 文档文件名 | 运行时值 | 通过 `Ctrl+O` 打开文件后的文件名 |
| SearchResWidget 搜索结果 | 搜索词不同 | 通过 `Form_SearchResWidget` 容器定位 |
| 进度对话框 | 进度%值 | 通过 `ProgressDialog` 类名定位 |
| 缩放下拉菜单 | 实时渲染 | 通过 `dtk_main_menu` 菜单操作 |
| 恢复提示条 | 动态显示 | 使用 `assert_element do: assert_not_exists` 验证消失 |