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
#include "controller/NotifySubject.h"
#include "subjectObserver/MsgHeader.h"
#include "utils/utils.h"
#include <DApplicationSettings>
#include <QIcon>
#include <QTranslator>
#include <QDebug>
#include "subjectObserver/ModuleHeader.h"

#include "controller/ObjectEventFilter.h"

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
    setOrganizationName(tr("deepin"));
    setWindowIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    setApplicationDisplayName(tr("reader"));
    setApplicationVersion(DApplication::buildVersion("20191125"));
    setApplicationAcknowledgementPage(Constant::sAcknowledgementLink);
    // setProductIcon(QIcon::fromTheme("deepin-reader"));
    QPixmap px(QIcon::fromTheme(ConstantMsg::g_app_name).pixmap(256 * qApp->devicePixelRatio(), 256 * qApp->devicePixelRatio()));
    px.setDevicePixelRatio(qApp->devicePixelRatio());
    setProductIcon(QIcon(px));


    setApplicationDescription(tr("Document viewer a reading tool for PDF documents."));

    installEventFilter(new ObjectEventFilter(this));

    initChildren();
}

void Application::handleQuitAction()
{
    NotifySubject::getInstance()->notifyMsg(MSG_OPERATION_EXIT);
}

void Application::initChildren()
{
    dbM = DBManager::instance();
    //   setter = ConfigSetter::instance();
}

void Application::initI18n()
{
    // install translators
    loadTranslator();
}
