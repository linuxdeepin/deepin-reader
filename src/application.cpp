#include "application.h"

#include <QIcon>
#include <QTranslator>
#include <QDebug>
#include <QDir>

#include "business/AppInfo.h"
#include "utils/utils.h"
#include "ModuleHeader.h"
#include "MsgHeader.h"
#include "MainWindow.h"
#include "djvuControl/RenderThreadDJVU.h"
#include "pdfControl/docview/pdf/RenderThreadPdf.h"
#include "DocSheet.h"
#include "business/SaveDialog.h"

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    setAttribute(Qt::AA_UseHighDpiPixmaps);
    setApplicationName(ConstantMsg::g_app_name);
    setOrganizationName("deepin");
    setWindowIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    setApplicationVersion(DApplication::buildVersion("1.0"));
    setApplicationAcknowledgementPage(Constant::sAcknowledgementLink);
    setApplicationDisplayName(tr("Document Viewer"));
    setApplicationDescription(tr("Document Viewer is a tool for reading document files, supporting PDF, DJVU, etc."));

    QPixmap px(QIcon::fromTheme(ConstantMsg::g_app_name).pixmap(static_cast<int>(256 * qApp->devicePixelRatio()), static_cast<int>(256 * qApp->devicePixelRatio())));
    px.setDevicePixelRatio(qApp->devicePixelRatio());
    setProductIcon(QIcon(px));

    initI18n();
    initCfgPath();
    initChildren();
}

void Application::setSreenRect(const QRect &rect)
{
    if (m_pAppInfo) {
        m_pAppInfo->setScreenRect(rect);
    }
}

void Application::handleQuitAction()
{
    QList<DocSheet *> changedList;

    foreach (auto sheet, DocSheet::g_map.values()) {
        if (sheet->fileChanged())
            changedList.append(sheet);
    }

    if (changedList.size() > 0) {   //需要提示保存
        SaveDialog sd;

        int nRes = sd.showDialog();

        if (nRes <= 0) {
            return;
        }

        if (nRes == 2) {
            foreach (auto sheet, changedList) {
                sheet->saveData();
            }
        }
    }

    //线程退出
    RenderThreadDJVU::destroy();
    RenderThreadPdf::destroyRenderPdfThread();

    foreach (MainWindow *window, MainWindow::m_list)
        window->close();
}

//  初始化 deepin-reader 的配置文件目录, 包含 数据库, conf.cfg
void Application::initCfgPath()
{
    QString sDbPath = Utils::getConfigPath();
    QDir dd(sDbPath);
    if (! dd.exists()) {
        dd.mkpath(sDbPath);
        if (dd.exists())
            qDebug() << __FUNCTION__ << "  create app dir succeed!";
    }
}

void Application::initChildren()
{
    m_pDBService = new DBService(this);
    m_pAppInfo = new AppInfo(this);
}

void Application::initI18n()
{
    loadTranslator();
}
