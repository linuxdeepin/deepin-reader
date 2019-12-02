#include "MainWidget.h"
#include <DSplitter>
#include <DSpinner>
#include <QFileInfo>
#include <QMimeData>
#include <QUrl>

#include "HomeWidget.h"
#include "DocShowShellWidget.h"
#include "LeftSidebarWidget.h"
#include "DocummentFileHelper.h"
#include "utils/utils.h"
#include <QStackedLayout>
#include <DDialog>
#include <DMessageManager>

MainWidget::MainWidget(CustomWidget *parent) :
    CustomWidget("MainWidget", parent)
{
    setAcceptDrops(true);

    initWidget();
    initConnections();
}

void MainWidget::initConnections()
{
    connect(this, SIGNAL(sigOpenFileOk()), SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigOpenFileStart()), SLOT(slotOpenFileStart()));
    connect(this, SIGNAL(sigOpenFileFail(const QString &)), SLOT(slotOpenFileFail(const QString &)));
    connect(this, SIGNAL(sigShowTips(const QString &)), SLOT(slotShowTips(const QString &)));
}

//  文件打开成功
void MainWidget::slotOpenFileOk()
{
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {
        pLayout->setCurrentIndex(1);
    }
}

void MainWidget::slotOpenFileStart()
{
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {
        pLayout->setCurrentIndex(2);
    }
}

//  文件打开失败
void MainWidget::slotOpenFileFail(const QString &errorInfo)
{
    //  文档打开失败, 切换回首页
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {
        if (pLayout->currentIndex() != 0) {
            pLayout->setCurrentIndex(0);
        }
    }
    DDialog dlg("", errorInfo);
    dlg.setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    dlg.addButtons(QStringList() << tr("Ok"));
    dlg.exec();
}

void MainWidget::slotShowTips(const QString &contant)
{
    DMessageManager::instance()->sendMessage(this, QIcon(":/resources/light/pdfControl/ok.svg"), contant);
}

int MainWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigOpenFileOk();
    } else if (msgType == MSG_OPERATION_OPEN_FILE_FAIL) {
        emit sigOpenFileFail(msgContent);
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_OPERATION_OPEN_FILE_START) {
        emit sigOpenFileStart();
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_NOTIFY_SHOW_TIP) {
        emit sigShowTips(msgContent);
        return ConstantMsg::g_effective_res;
    }

    return 0;
}

void MainWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept drag event if mime type is url.
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->accept();
    }
}

void MainWidget::dropEvent(QDropEvent *event)
{
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {

        QStringList noOpenFileList;
        QStringList canOpenFileList;

        for (auto url : mimeData->urls()) {
            QString sFilePath =  url.toLocalFile();

            QFileInfo file(sFilePath);
            if (file.isFile()) {
                QString sSuffix = file.completeSuffix();
                if (sSuffix == "pdf" || sFilePath.endsWith(QString(".pdf"))) {    //  打开第一个pdf文件
                    canOpenFileList.append(sFilePath);
                } else {
                    if (!noOpenFileList.contains(sSuffix)) {
                        noOpenFileList.append(sSuffix);
                    }
                }
            }
        }

        foreach (auto s, noOpenFileList) {
            QString msgType = s;
            if (msgType == "") {
                msgType = tr("unknown type");
            }
            QString msgTitle = QString("%1").arg(msgType);
            QString msgContent = msgTitle + tr(" is not supported.");
            slotShowTips(msgContent);
        }
        bool bisopen = false;
        foreach (auto s, canOpenFileList) {
            QString sRes = s + Constant::sQStringSep;
            if (nullptr != DocummentProxy::instance() && !DocummentProxy::instance()->isOpendFile()) {
                if (!bisopen)
                    notifyMsg(MSG_OPEN_FILE_PATH, sRes);
                else {
                    if (!Utils::runApp(s))
                        qDebug() << __FUNCTION__ << "process start deepin-reader failed";
                }

                bisopen = true;
            } else {
                Utils::runApp(s);
            }
        }
    }
}

void MainWidget::initWidget()
{
    auto pStcakLayout = new QStackedLayout(this);
    pStcakLayout->setContentsMargins(0, 0, 0, 0);
    pStcakLayout->setSpacing(0);

    pStcakLayout->addWidget(new HomeWidget);

    auto pSplitter = new DSplitter;
    pSplitter->setHandleWidth(5);
    pSplitter->setChildrenCollapsible(false);   //  子部件不可拉伸到 0

    pSplitter->addWidget(new LeftSidebarWidget);
    pSplitter->addWidget(new DocShowShellWidget);

    QList<int> list_src;
    list_src.append(226);
    list_src.append(1000);

    pSplitter->setSizes(list_src);

    pStcakLayout->addWidget(pSplitter);

    auto pSpinnerWidget = new DWidget;
    QGridLayout *gridlyout = new QGridLayout(pSpinnerWidget);
    gridlyout->setAlignment(Qt::AlignCenter);
    auto m_spinner = new DSpinner;
    m_spinner->setFixedSize(60, 60);
    gridlyout->addWidget(m_spinner);
    m_spinner->start();
    pStcakLayout->addWidget(pSpinnerWidget);
}
