/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "application.h"

#include <QIcon>
#include <QTranslator>
#include <QDebug>
#include <QDir>

#include "subjectObserver/ModuleHeader.h"
#include "subjectObserver/MsgHeader.h"
#include "controller/NotifySubject.h"
#include "utils/utils.h"

namespace {

}  // namespace

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    setAttribute(Qt::AA_UseHighDpiPixmaps);
    // setAttribute(Qt::AA_EnableHighDpiScaling);
    // setAttribute(Qt::AA_ForceRasterWidgets);

    initI18n();

    setApplicationName(ConstantMsg::g_app_name);
    setOrganizationName("deepin");
    setWindowIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    setApplicationDisplayName(tr("Document Viewer"));
    setApplicationVersion(DApplication::buildVersion("20191227"));
    setApplicationAcknowledgementPage(Constant::sAcknowledgementLink);

    QPixmap px(QIcon::fromTheme(ConstantMsg::g_app_name).pixmap(256 * qApp->devicePixelRatio(), 256 * qApp->devicePixelRatio()));
    px.setDevicePixelRatio(qApp->devicePixelRatio());
    setProductIcon(QIcon(px));

    //  wzx 2019-12-21 根据石墨文案修改
    setApplicationDescription(tr("Document Viewer is a simple PDF reader, supporting bookmarks, highlights and annotations."));
    //kyz 2019-12-10 不允许在此处安装事件过滤
    // installEventFilter(new ObjectEventFilter(this));

    initCfgPath();

    initChildren();

    g_NotifySubject::getInstance()->startThreadRun();
}

Application::~Application()
{
    histroyDb->deleteLater();
    histroyDb = nullptr;

    g_NotifySubject::getInstance()->stopThreadRun();
}

void Application::handleQuitAction()
{
    g_NotifySubject::getInstance()->notifyMsg(MSG_OPERATION_EXIT);
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
    dbM = new DBManager(this);
    histroyDb = new HistroyDB(this);
}

void Application::initI18n()
{
    // install translators
    loadTranslator();
}
