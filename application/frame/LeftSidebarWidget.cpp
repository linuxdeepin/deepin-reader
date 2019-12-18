#include "LeftSidebarWidget.h"
#include <DStackedWidget>
#include <QButtonGroup>
#include <QVBoxLayout>

#include "controller/DataManager.h"
#include "pdfControl/BookMarkWidget.h"
#include "pdfControl/BufferWidget.h"
#include "pdfControl/NotesWidget.h"
#include "pdfControl/SearchResWidget.h"
#include "pdfControl/ThumbnailWidget.h"

#include "MainOperationWidget.h"
#include "controller/AppSetting.h"

LeftSidebarWidget::LeftSidebarWidget(CustomWidget *parent)
    : CustomWidget("LeftSidebarWidget", parent)
{
    resize(LEFTNORMALWIDTH, this->height());
    setMinimumWidth(LEFTMINWIDTH);
    setMaximumWidth(LEFTMAXWIDTH);

    initWidget();
    initConnections();

    slotWidgetVisible(0);  //  默认 隐藏
    slotUpdateTheme();
}

void LeftSidebarWidget::slotStackSetCurIndex(const int &iIndex)
{
    AppSetting::instance()->setKeyValue(KEY_WIDGET, QString("%1").arg(iIndex));

    auto pWidget = this->findChild<DStackedWidget *>();
    if (pWidget) {
        if (WIDGET_SEARCH > iIndex) {
            DataManager::instance()->setCurrentWidget(iIndex);
            auto opWidget = this->findChild<MainOperationWidget *>();
            if (opWidget) {
                opWidget->setOperatAction(iIndex);
            }
        }
        pWidget->setCurrentIndex(iIndex);
    }
}

void LeftSidebarWidget::slotWidgetVisible(const int &nVis)
{
    this->setVisible(nVis);
}

void LeftSidebarWidget::slotUpdateTheme()
{
    updateWidgetTheme();
}

//  上一页
void LeftSidebarWidget::slotJumpToPrevPage(const QString &msgContent)
{
    //  1.需要判断当前的模式, 是正常显示 还是幻灯片模式
    //  2.若不是幻灯片模式, 则正常进行处理
    qDebug() << __FUNCTION__ << "           " << msgContent;
    bool bl = this->isVisible();
    if (bl) {
        auto pWidget = this->findChild<DStackedWidget *>();
        if (pWidget) {
            int iIndex = pWidget->currentIndex();
            doPrevPage(iIndex);
//            emit sigJumpToPrevPage(iIndex, msgContent);
        }
    } else {
        forScreenPageing(false);
    }
}

//  下一页
void LeftSidebarWidget::slotJumpToNextPage(const QString &msgContent)
{
    //  1.需要判断当前的模式, 是正常显示 还是幻灯片模式
    //  2.若不是幻灯片模式, 则正常进行处理
    qDebug() << __FUNCTION__ << "           " << msgContent;
    bool bl = this->isVisible();
    if (bl) {
        auto pWidget = this->findChild<DStackedWidget *>();
        if (pWidget) {
            int iIndex = pWidget->currentIndex();
            doNextPage(iIndex);
//            emit sigJumpToNextPage(iIndex, msgContent);
        }
    } else {
        forScreenPageing(true);
    }
}

void LeftSidebarWidget::initConnections()
{
    connect(this, SIGNAL(sigStackSetCurIndex(const int &)),
            SLOT(slotStackSetCurIndex(const int &)));
    connect(this, SIGNAL(sigWidgetVisible(const int &)), SLOT(slotWidgetVisible(const int &)));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));

    connect(this, SIGNAL(sigJumpToPrevPage(const QString &)), this, SLOT(slotJumpToPrevPage(const QString &)));
    connect(this, SIGNAL(sigJumpToNextPage(const QString &)), this, SLOT(slotJumpToNextPage(const QString &)));
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
    pStackedWidget->insertWidget(WIDGET_BOOKMARK, new BookMarkWidget(this));
    pStackedWidget->insertWidget(WIDGET_NOTE, new NotesWidget(this));
    pStackedWidget->insertWidget(WIDGET_SEARCH, new SearchResWidget(this));
    pStackedWidget->insertWidget(WIDGET_BUFFER, new BufferWidget(this));
    pStackedWidget->setCurrentIndex(WIDGET_THUMBNAIL);

    for (int index = 0; index < pStackedWidget->count(); ++index) {
        auto widget = pStackedWidget->widget(index);
        if (widget) {
            widget->setMinimumWidth(LEFTMINWIDTH);
            widget->setMaximumWidth(LEFTMAXWIDTH);
            widget->adjustSize();
            widget->resize(226, this->height());
        }
    }

    pVBoxLayout->addWidget(pStackedWidget);
    pVBoxLayout->addWidget(new MainOperationWidget, 0, Qt::AlignBottom);
}

int LeftSidebarWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_SWITCHLEFTWIDGET) {  //切换页面
        emit sigStackSetCurIndex(msgContent.toInt());
        return ConstantMsg::g_effective_res;
    }
    if (msgType == MSG_SLIDER_SHOW_STATE) {  //  控制 侧边栏显隐
        emit sigWidgetVisible(msgContent.toInt());
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    } else if (MSG_NOTIFY_KEY_MSG == msgType) {
        //  上 下 一页 由左侧栏进行转发
        if (msgContent == KeyStr::g_up || msgContent == KeyStr::g_pgup ||
                msgContent == KeyStr::g_left) {
            emit sigJumpToPrevPage(msgContent);
        } else if (msgContent == KeyStr::g_down || msgContent == KeyStr::g_pgdown ||
                   msgContent == KeyStr::g_right) {
            emit sigJumpToNextPage(msgContent);
        }
    }

    return 0;
}

void LeftSidebarWidget::resizeEvent(QResizeEvent *event)
{
    CustomWidget::resizeEvent(event);

    update();
}
