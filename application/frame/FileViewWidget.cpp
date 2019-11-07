#include "FileViewWidget.h"
#include <QGridLayout>
#include <QClipboard>
#include "controller/DataManager.h"
#include "application.h"

#include "docview/docummentproxy.h"
#include <DMessageBox>

#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintPreviewDialog>

#include "mainShow/DefaultOperationMenu.h"

FileViewWidget::FileViewWidget(CustomWidget *parent)
    : CustomWidget("FileViewWidget", parent)
    , m_operatemenu(nullptr)
{
    setMouseTracking(true); //  接受 鼠标滑动事件
    setContextMenuPolicy(Qt::CustomContextMenu);

    initWidget();
    initConnections();
}

FileViewWidget::~FileViewWidget()
{

}

void FileViewWidget::initWidget()
{
    //  实际文档类  唯一实例化设置 父窗口
    auto m_pDocummentProxy = DocummentProxy::instance(this);
    connect(m_pDocummentProxy, SIGNAL(signal_bookMarkStateChange(int, bool )),
            this, SLOT(slotBookMarkStateChange(int, bool)));
    connect(m_pDocummentProxy, SIGNAL(signal_pageChange(int)),
            this, SLOT(slotDocFilePageChanged(int)));

    m_pDocummentFileHelper = DocummentFileHelper::instance();
    connect(this, SIGNAL(sigOpenFile(const QString &)), m_pDocummentFileHelper, SLOT(slotOpenFile(const QString &)));
    connect(this, SIGNAL(sigSaveFile()), m_pDocummentFileHelper, SLOT(slotSaveFile()));
    connect(this, SIGNAL(sigSaveAsFile()), m_pDocummentFileHelper, SLOT(slotSaveAsFile()));
    connect(this, SIGNAL(sigFileSlider(const int &)), m_pDocummentFileHelper, SLOT(slotFileSlider(const int &)));
    connect(this, SIGNAL(sigCopySelectContent(const QString &)), m_pDocummentFileHelper, SLOT(slotCopySelectContent(const QString &)));
}

//  鼠标移动
void FileViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
        return;
    }

    QPoint globalPos = event->globalPos();
    QPoint docGlobalPos = m_pDocummentFileHelper->global2RelativePoint(globalPos);
    if (m_nCurrentHandelState == Handel_State) {    //   手型状态下， 按住鼠标左键 位置进行移动
        if (m_bSelectOrMove) {
            QPoint mvPoint = m_pHandleMoveStartPoint - globalPos;
            int mvX = mvPoint.x();
            int mvY = mvPoint.y();

            m_pDocummentFileHelper->pageMove(mvX, mvY);

            m_pHandleMoveStartPoint = globalPos;
        }
    } else if (m_nCurrentHandelState == Magnifier_State) {  //  当前是放大镜状态
        m_pDocummentFileHelper->showMagnifier(docGlobalPos);
    } else {
        if (m_bSelectOrMove) {  //  鼠标已经按下，　则选中所经过的文字
            m_pDocummentFileHelper->mouseSelectText(m_pStartPoint, docGlobalPos);
        } else {
            //  首先判断文档划过属性
            Page::Link *pLink  = m_pDocummentFileHelper->mouseBeOverLink(docGlobalPos);
            if (pLink) {
                setCursor(QCursor(Qt::PointingHandCursor));
            } else {
                if (m_pDocummentFileHelper->mouseBeOverText(docGlobalPos)) {
                    m_bIsHandleSelect = true;
                    setCursor(QCursor(Qt::IBeamCursor));
                } else {
                    m_bIsHandleSelect = false;
                    setCursor(QCursor(Qt::ArrowCursor));
                }
            }
        }
    }
}

//  鼠标左键 按下
void FileViewWidget::mousePressEvent(QMouseEvent *event)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
        return;
    }
    //  放大镜状态， 直接返回
    if (m_nCurrentHandelState == Magnifier_State)
        return;

    Qt::MouseButton nBtn = event->button();
    if (nBtn == Qt::LeftButton) {
        QPoint globalPos = event->globalPos();

        //  当前状态是 手, 先 拖动, 之后 在是否是链接之类
        if (m_nCurrentHandelState == Handel_State) {
            m_pHandleMoveStartPoint = globalPos;     //  变成手，　需要的是　相对坐标
            m_bSelectOrMove = true;
        } else {
            QPoint docGlobalPos = m_pDocummentFileHelper->global2RelativePoint(globalPos);

            //  点击的时候　先判断　点击处　　是否有链接之类
            Page::Link *pLink = m_pDocummentFileHelper->mouseBeOverLink(docGlobalPos);
            if (pLink) {
                m_pDocummentFileHelper->onClickPageLink(pLink);
            } else {
                if (m_nCurrentHandelState == Default_State) {
                    m_pDocummentFileHelper->mouseSelectTextClear();  //  清除之前选中的文字高亮
                    if (m_bIsHandleSelect) {
                        m_bSelectOrMove = true;
                        m_pStartPoint = docGlobalPos;
                    }
                }
            }
        }
    }
}



//  鼠标松开
void FileViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE)
        return;

    if (event->button() == Qt::RightButton && m_nCurrentHandelState == Magnifier_State) {
        emit sigMagnifying(QString("0"));
        return;
    }
    //  放大镜状态， 直接返回
    if (m_nCurrentHandelState == Magnifier_State)
        return;

    if (m_bSelectOrMove) {
        //判断鼠标左键松开的位置有没有高亮
        Qt::MouseButton nBtn = event->button();
        if (nBtn == Qt::LeftButton) {
            QPoint globalPos = event->globalPos();
            QPoint docGlobalPos = m_pDocummentFileHelper->global2RelativePoint(globalPos);
            // 判断鼠标点击的地方是否有高亮
            QString selectText, t_strUUid;

            bool bIsHighLightReleasePoint = m_pDocummentFileHelper->annotationClicked(docGlobalPos, selectText, t_strUUid);
            DataManager::instance()->setMousePressLocal(bIsHighLightReleasePoint, globalPos);
            if (bIsHighLightReleasePoint) {
                int nPage = m_pDocummentFileHelper->pointInWhichPage(docGlobalPos);
                QString t_strContant = t_strUUid.trimmed() + QString("%1%") + QString::number(nPage);
                sendMsg(MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, t_strContant);
            }
        }
    }

    m_bSelectOrMove = false;
    CustomWidget::mouseReleaseEvent(event);
}

//  文档 显示区域 大小变化
void FileViewWidget::resizeEvent(QResizeEvent *event)
{
    slotSetWidgetAdapt();

    CustomWidget::resizeEvent(event);
}

//  弹出 自定义 菜单
void FileViewWidget::slotCustomContextMenuRequested(const QPoint &point)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE)
        return;

    //  放大镜状态， 直接返回
    if (m_nCurrentHandelState == Magnifier_State)
        return;

    //  手型状态， 直接返回
    if (m_nCurrentHandelState == Handel_State)
        return;


    QString sSelectText =  "";
    m_pDocummentFileHelper->getSelectTextString(sSelectText);  //  选择　当前选中下面是否有文字

    QPoint tempPoint = this->mapToGlobal(point);
    QPoint pRightClickPoint = m_pDocummentFileHelper->global2RelativePoint(tempPoint);

    //  右键鼠标点 是否有高亮区域
    QString sAnnotationText = "", struuid = "";
    bool bIsHighLight = m_pDocummentFileHelper->annotationClicked(pRightClickPoint, sAnnotationText, struuid);

    int nPage = m_pDocummentFileHelper->pointInWhichPage(pRightClickPoint);
    if (sSelectText != "" || bIsHighLight) {    //  选中区域 有文字, 弹出 文字操作菜单
        //  需要　区别　当前选中的区域，　弹出　不一样的　菜单选项
        if (nullptr == m_operatemenu) {
            m_operatemenu = new TextOperationMenu(this);
        }
        m_operatemenu->setClickPoint(pRightClickPoint);
        m_operatemenu->setClickPage(nPage);

        m_operatemenu->execMenu(tempPoint, bIsHighLight, sSelectText, struuid);
    } else {    //  否则弹出 文档操作菜单
        auto menu = new DefaultOperationMenu(this);
        menu->execMenu(tempPoint, nPage);
    }
}

//  放大镜　控制
void FileViewWidget::slotMagnifying(const QString &data)
{
    int nRes = data.toInt();
    if (nRes == 1) {
        m_pDocummentFileHelper->mouseSelectTextClear();  //  清除之前选中的文字高亮

        m_nCurrentHandelState = Magnifier_State;
        this->setCursor(Qt::BlankCursor);
    } else {
        if (m_nCurrentHandelState == Magnifier_State) { //  是 放大镜模式 才取消

            m_nCurrentHandelState = Default_State;
            this->setCursor(Qt::ArrowCursor);

            m_pDocummentFileHelper->closeMagnifier();
        }
    }
}

//  手势控制
void FileViewWidget::slotSetHandShape(const QString &data)
{
    int nRes = data.toInt();
    if (nRes == 1) { //  手形
        m_nCurrentHandelState = Handel_State;
        this->setCursor(Qt::OpenHandCursor);
    } else {
        m_nCurrentHandelState = Default_State;
        this->setCursor(Qt::ArrowCursor);
    }

    //  手型 切换 也需要将之前选中的文字清除 选中样式
    m_pDocummentFileHelper->mouseSelectTextClear();
}

//  添加高亮颜色
void FileViewWidget::slotFileAddAnnotation(const QString &msgContent)
{
    QStringList contentList = msgContent.split(",", QString::SkipEmptyParts);
    if (contentList.size() == 3) {
        QString sIndex = contentList.at(0);
        QString sX = contentList.at(1);
        QString sY = contentList.at(2);

        int iIndex = sIndex.toInt();
        QColor color = DataManager::instance()->getLightColorList().at(iIndex);

        QPoint tempPoint(sX.toInt(), sY.toInt());

        QString sUuid = m_pDocummentFileHelper->addAnnotation(tempPoint, tempPoint, color);
        if (sUuid != "") {
            DataManager::instance()->setBIsUpdate(true);
        }
    }
}

//  移除高亮, 有注释 则删除注释
void FileViewWidget::slotFileRemoveAnnotation(const QString &msgContent)
{
    QStringList contentList = msgContent.split(",", QString::SkipEmptyParts);
    if (contentList.size() == 2) {
        QString sX = contentList.at(0);
        QString sY = contentList.at(1);

        QPoint tempPoint(sX.toInt(), sY.toInt());
        QString sUuid = m_pDocummentFileHelper->removeAnnotation(tempPoint);
        if (sUuid != "") {
            sendMsg(MSG_NOTE_DLTNOTEITEM, sUuid);

            m_pDocummentFileHelper->removeAnnotation(sUuid);

            DataManager::instance()->setBIsUpdate(true);
        }
    }
}

//  添加注释
void FileViewWidget::slotFileAddNote(const QString &msgContent)
{
    QString sUuid = "", sNote = "", sPage = "";

    QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (contentList.size() == 3) {
        sNote = contentList.at(0);
        sUuid = contentList.at(1);
        sPage = contentList.at(2);
    } else if (contentList.size() == 4) {
        sNote = contentList.at(0);
        sPage = contentList.at(1);
        QString sX = contentList.at(2);
        QString sY = contentList.at(3);

        QColor color = DataManager::instance()->getLightColorList().at(0);
        QPoint tempPoint(sX.toInt(), sY.toInt());

        sUuid = m_pDocummentFileHelper->addAnnotation(tempPoint, tempPoint, color);  //  高亮 产生的 uuid
    }

    if (sUuid == "" || sNote == "" || sPage == "" || sPage == "-1") {
        return;
    }
    QString t_str = sUuid.trimmed() + QString("%") + sNote.trimmed() + QString("%") + sPage;
    sendMsg(MSG_NOTE_ADDITEM, t_str);
    m_pDocummentFileHelper->setAnnotationText(sPage.toInt(), sUuid, sNote);

    DataManager::instance()->setBIsUpdate(true);
}

//  文档书签状态改变
void FileViewWidget::slotBookMarkStateChange(int nPage, bool bState)
{
    if (!bState) {
        sendMsg(MSG_OPERATION_DELETE_BOOKMARK, QString("%1").arg(nPage));
    } else {
        sendMsg(MSG_OPERATION_ADD_BOOKMARK, QString("%1").arg(nPage));
    }
}

//  文档页变化了
void FileViewWidget::slotDocFilePageChanged(int page)
{
    sendMsg(MSG_FILE_PAGE_CHANGE, QString("%1").arg(page));
}

//  信号槽　初始化
void FileViewWidget::initConnections()
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(slotCustomContextMenuRequested(const QPoint &)));

    connect(this, SIGNAL(sigSetHandShape(const QString &)), this, SLOT(slotSetHandShape(const QString &)));
    connect(this, SIGNAL(sigMagnifying(const QString &)), this, SLOT(slotMagnifying(const QString &)));
    connect(this, SIGNAL(sigWidgetAdapt()), this, SLOT(slotSetWidgetAdapt()));
    connect(this, SIGNAL(sigPrintFile()), this, SLOT(slotPrintFile()));

    connect(this, SIGNAL(sigFileAddAnnotation(const QString &)), this, SLOT(slotFileAddAnnotation(const QString &)));
    connect(this, SIGNAL(sigFileRemoveAnnotation(const QString &)), this, SLOT(slotFileRemoveAnnotation(const QString &)));

    connect(this, SIGNAL(sigFileAddNote(const QString &)), this, SLOT(slotFileAddNote(const QString &)));
}

//  打印
void FileViewWidget::slotPrintFile()
{
    QPrinter printer(QPrinter::ScreenResolution);
    // 创建打印对话框
    QString printerName = printer.printerName();
    if ( printerName.size() == 0) {
        DMessageBox::warning(this, tr("Print Error"), tr("No Print Device"));
        return;
    }

    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, [ = ] (QPrinter * printer) {

        int nPageSize = m_pDocummentFileHelper->getPageSNum();       //  pdf 页数
        printer->setWinPageSize(nPageSize);

        QPainter painter(printer);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.begin(printer);
        QRect rect = painter.viewport();

        for (int iIndex = 0; iIndex < nPageSize; iIndex++) {
            QImage image;

            bool rl = m_pDocummentFileHelper->getImage(iIndex, image, 800, 1100);
            if (rl) {
                QPixmap pixmap = pixmap.fromImage(image);

                QSize size = pixmap.size();
                size.scale(rect.size(), Qt::KeepAspectRatio);//此处保证图片显示完整
                painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
                painter.setWindow(pixmap.rect());
                painter.drawPixmap(10, 10, 800, 1100, pixmap);
                if (iIndex < nPageSize - 1)
                    printer->newPage();
            }
        }
        painter.end();
    });

    preview.exec();
}

//  设置　窗口　自适应　宽＼高　度
void FileViewWidget::slotSetWidgetAdapt()
{
    double nScale = 0.0;
    if (m_nAdapteState == WIDGET_State) {
        int nWidth = this->width();
        nScale = m_pDocummentFileHelper->adaptWidthAndShow(nWidth);
    } else if (m_nAdapteState == HEIGHT_State) {
        int nHeight = this->height();
        nScale = m_pDocummentFileHelper->adaptHeightAndShow(nHeight);
    }

    if (nScale != 0.0) {
        sendMsg(MSG_SELF_ADAPTE_SCALE, QString::number(nScale));
    }
}

//  标题栏的菜单消息处理
int FileViewWidget::dealWithTitleRequest(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_MAGNIFYING:            //  放大镜信号
        emit sigMagnifying(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_HANDLESHAPE:           //  手势 信号
        emit sigSetHandShape(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_SELF_ADAPTE_HEIGHT:    //  自适应　高度
        if (msgContent == "1") {
            m_nAdapteState = HEIGHT_State ;
            emit sigWidgetAdapt();
        } else {
            m_nAdapteState = Default_State;
        }
        return ConstantMsg::g_effective_res;
    case MSG_SELF_ADAPTE_WIDTH:    //   自适应宽度
        if (msgContent == "1") {
            m_nAdapteState = WIDGET_State;
            emit sigWidgetAdapt();
        } else {
            m_nAdapteState = Default_State;
        }
        return ConstantMsg::g_effective_res;
    case MSG_FILE_ROTATE:           //  文档旋转了
        emit sigWidgetAdapt();
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

//  文档右键菜单请求处理
int FileViewWidget::dealWithFileMenuRequest(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPEN_FILE_PATH:                    //  打开文件
        emit sigOpenFile(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SAVE_FILE :              //  保存文件
        emit sigSaveFile();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SAVE_AS_FILE:            //  另存为文件
        emit sigSaveAsFile();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_COPY:               //  复制
        emit sigCopySelectContent(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SLIDE:                   //  放映
        emit sigFileSlider(1);
        break;
    case MSG_OPERATION_PRINT :                  //  打印
        emit sigPrintFile();
        return  ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_ADD_HIGHLIGHTED:    //  高亮显示
        emit sigFileAddAnnotation(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_REMOVE_HIGHLIGHTED: //  移除高亮显示
        emit sigFileRemoveAnnotation(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_NOTE_ADDCONTANT:                   //  添加注释
        emit sigFileAddNote(msgContent);
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

//  集中处理 按键通知消息
int FileViewWidget::dealWithNotifyMsg(const QString &msgContent)
{
    if (KeyStr::g_ctrl_s == msgContent) {
        emit sigSaveFile();
        return ConstantMsg::g_effective_res;
    }

    if (msgContent == KeyStr::g_ctrl_p) {
        emit sigPrintFile();
        return ConstantMsg::g_effective_res;
    }

    if (msgContent == KeyStr::g_ctrl_shift_s) {
        emit sigSaveAsFile();
        return ConstantMsg::g_effective_res;
    }

    if (KeyStr::g_esc == msgContent) {
        emit sigFileSlider(0);
    }

    return 0;
}

//  消息 数据 处理
int FileViewWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    int nRes = dealWithTitleRequest(msgType, msgContent);
    if (nRes != ConstantMsg::g_effective_res) {

        nRes = dealWithFileMenuRequest(msgType, msgContent);
        if (nRes != ConstantMsg::g_effective_res) {
            if (msgType == MSG_NOTIFY_KEY_MSG) {
                nRes = dealWithNotifyMsg(msgContent);
            }
        }
    }

    return nRes;
}
