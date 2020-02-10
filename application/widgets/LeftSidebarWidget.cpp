#include "LeftSidebarWidget.h"

#include <DStackedWidget>
#include <QButtonGroup>
#include <QVBoxLayout>

#include "MainOperationWidget.h"

#include "controller/DataManager.h"
#include "controller/AppSetting.h"
#include "docview/docummentproxy.h"

#include "pdfControl/bookmark/BookMarkWidget.h"
#include "pdfControl/catalog/CatalogWidget.h"
#include "pdfControl/note/NotesWidget.h"
#include "pdfControl/search/BufferWidget.h"
#include "pdfControl/search/SearchResWidget.h"
#include "pdfControl/thumbnail/ThumbnailWidget.h"

LeftSidebarWidget::LeftSidebarWidget(CustomWidget *parent)
    : CustomWidget("LeftSidebarWidget", parent)
{
    setMinimumWidth(LEFTMINWIDTH);
    setMaximumWidth(LEFTMAXWIDTH);

    resize(LEFTNORMALWIDTH, this->height());

    m_pMsgList = {MSG_SLIDER_SHOW_STATE};
    m_pKeyMsgList = {KeyStr::g_up, KeyStr::g_pgup, KeyStr::g_left,
                     KeyStr::g_down, KeyStr::g_pgdown, KeyStr::g_right,
                     KeyStr::g_del
                    };

    initWidget();
    initConnections();

    onSetWidgetVisible(0);  //  默认 隐藏
    slotUpdateTheme();

    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

LeftSidebarWidget::~LeftSidebarWidget()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

void LeftSidebarWidget::slotDealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_SLIDER_SHOW_STATE) {//  控制 侧边栏显隐
        onSetWidgetVisible(msgContent.toInt());
    }
}

void LeftSidebarWidget::slotDealWithKeyMsg(const QString &msgContent)
{
    //  上 下 一页 由左侧栏进行转发
    if (msgContent == KeyStr::g_up || msgContent == KeyStr::g_pgup ||
            msgContent == KeyStr::g_left) {
        onJumpToPrevPage();
    } else if (msgContent == KeyStr::g_down || msgContent == KeyStr::g_pgdown ||
               msgContent == KeyStr::g_right) {
        onJumpToNextPage();
    } else if (msgContent == KeyStr::g_del) {
        __DeleteItemByKey();
    }
}

void LeftSidebarWidget::onSetWidgetVisible(const int &nVis)
{
    this->setVisible(nVis);
    if (!nVis && DocummentProxy::instance())
        DocummentProxy::instance()->setViewFocus();
}

void LeftSidebarWidget::slotUpdateTheme()
{
    updateWidgetTheme();
}

//  按钮 按键显示对应 widget
void LeftSidebarWidget::slotSetStackCurIndex(const int &iIndex)
{
    auto pWidget = this->findChild<DStackedWidget *>();
    if (pWidget) {
        pWidget->setCurrentIndex(iIndex);
    }

    //  前一个是 出来搜索结果了, 后一个是正在搜索, 两个都不需要保存在记录中
    if (iIndex == WIDGET_SEARCH || iIndex == WIDGET_BUFFER) {
        emit sigSearchWidgetState(iIndex);
    } else {
        DataManager::instance()->setListIndex(QString::number(iIndex));
    }
}

//  上一页
void LeftSidebarWidget::onJumpToPrevPage()
{
    //  1.需要判断当前的模式, 是正常显示 还是幻灯片模式
    //  2.若不是幻灯片模式, 则正常进行处理
    bool bl = this->isVisible();
    if (bl) {
        auto pWidget = this->findChild<DStackedWidget *>();
        if (pWidget) {
            int iIndex = pWidget->currentIndex();
            doPrevPage(iIndex);
        }
    } else {
        forScreenPageing(false);
    }
}

//  下一页
void LeftSidebarWidget::onJumpToNextPage()
{
    //  1.需要判断当前的模式, 是正常显示 还是幻灯片模式
    //  2.若不是幻灯片模式, 则正常进行处理
    bool bl = this->isVisible();
    if (bl) {
        auto pWidget = this->findChild<DStackedWidget *>();
        if (pWidget) {
            int iIndex = pWidget->currentIndex();
            doNextPage(iIndex);
        }
    } else {
        forScreenPageing(true);
    }
}

void LeftSidebarWidget::initConnections()
{
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));

    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)), SLOT(slotDealWithData(const int &, const QString &)));
    connect(this, SIGNAL(sigDealWithKeyMsg(const QString &)), SLOT(slotDealWithKeyMsg(const QString &)));
}

//  按delete 键 删除书签 或者 注释
void LeftSidebarWidget::__DeleteItemByKey()
{
    bool bl = this->isVisible();
    if (bl) {
        auto pWidget = this->findChild<DStackedWidget *>();
        if (pWidget) {
            int iIndex = pWidget->currentIndex();
            if (iIndex == WIDGET_BOOKMARK) {
                auto widget = this->findChild<BookMarkWidget *>();
                if (widget) {
                    widget->DeleteItemByKey();
                }
            } else if (iIndex == WIDGET_NOTE) {
                auto widget = this->findChild<NotesWidget *>();
                if (widget) {
                    widget->DeleteItemByKey();
                }
            }
        }
    }
}

/**
 * @brief LeftSidebarWidget::doPrevPage
 * 上一页
 * @param index
 */
void LeftSidebarWidget::doPrevPage(const int &index)
{
    if (index == WIDGET_THUMBNAIL) {
        auto widget = this->findChild<ThumbnailWidget *>();
        if (widget) {
            widget->prevPage();
        }
    }  else if (index == WIDGET_BOOKMARK) {
        auto widget = this->findChild<BookMarkWidget *>();
        if (widget) {
            widget->prevPage();
        }
    } else if (index == WIDGET_NOTE) {
        auto widget = this->findChild<NotesWidget *>();
        if (widget) {
            widget->prevPage();
        }
    }
}

/**
 * @brief LeftSidebarWidget::doNextPage
 * 下一页
 * @param index
 */
void LeftSidebarWidget::doNextPage(const int &index)
{
    if (index == WIDGET_THUMBNAIL) {
        auto widget = this->findChild<ThumbnailWidget *>();
        if (widget) {
            widget->nextPage();
        }
    }  else if (index == WIDGET_BOOKMARK) {
        auto widget = this->findChild<BookMarkWidget *>();
        if (widget) {
            widget->nextPage();
        }
    } else if (index == WIDGET_NOTE) {
        auto widget = this->findChild<NotesWidget *>();
        if (widget) {
            widget->nextPage();
        }
    }
}

/**
 * @brief LeftSidebarWidget::forScreenPageing
 * 全屏和放映时快捷键切换页 true:向下翻页  false:向上翻页
 */
void LeftSidebarWidget::forScreenPageing(bool direction)
{
    auto widget = this->findChild<ThumbnailWidget *>();
    if (widget) {
        widget->forScreenPageing(direction);
    }
}

void LeftSidebarWidget::initWidget()
{
    auto pVBoxLayout = new QVBoxLayout;
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->setSpacing(0);
    this->setLayout(pVBoxLayout);

    auto pStackedWidget = new DStackedWidget;
    pStackedWidget->insertWidget(WIDGET_THUMBNAIL, new ThumbnailWidget(this));
    pStackedWidget->insertWidget(WIDGET_catalog, new CatalogWidget(this));
    pStackedWidget->insertWidget(WIDGET_BOOKMARK, new BookMarkWidget(this));
    pStackedWidget->insertWidget(WIDGET_NOTE, new NotesWidget(this));
    pStackedWidget->insertWidget(WIDGET_SEARCH, new SearchResWidget(this));

    auto buffWidget = new BufferWidget(this);
    connect(this, SIGNAL(sigSearchWidgetState(const int &)), buffWidget, SLOT(SlotSetSpinnerState(const int &)));

    pStackedWidget->insertWidget(WIDGET_BUFFER, buffWidget);
    pStackedWidget->setCurrentIndex(WIDGET_THUMBNAIL);

    pVBoxLayout->addWidget(pStackedWidget);

    auto mainOperation = new MainOperationWidget;
    connect(mainOperation, SIGNAL(sigShowStackWidget(const int &)), SLOT(slotSetStackCurIndex(const int &)));

    pVBoxLayout->addWidget(mainOperation, 0, Qt::AlignBottom);
}

int LeftSidebarWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    } else if (MSG_NOTIFY_KEY_MSG == msgType) {
        if (m_pKeyMsgList.contains(msgContent)) {
            emit sigDealWithKeyMsg(msgContent);
            return ConstantMsg::g_effective_res;
        }
    }

    return 0;
}

void LeftSidebarWidget::resizeEvent(QResizeEvent *event)
{
    CustomWidget::resizeEvent(event);

    update();
}
