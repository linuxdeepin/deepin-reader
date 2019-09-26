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

#include "controller/GlobalEventFilter.h"

#include "controller/MsgSubject.h"
#include "subjectObserver/MsgHeader.h"

#include <DApplicationSettings>
#include <QIcon>
#include <QTranslator>
#include <QDebug>
#include "frame/AppAboutWidget.h"
#include "translator/Frame.h"

namespace {

}  // namespace

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    setAttribute(Qt::AA_UseHighDpiPixmaps);
    setAttribute(Qt::AA_EnableHighDpiScaling);
    setAttribute(Qt::AA_ForceRasterWidgets);

    initI18n();

    setOrganizationName(Frame::ORGANIZATION_NAME);
    setApplicationName(Frame::sAppName);
    setApplicationDisplayName(Frame::sAppName);
    setApplicationVersion("1.0");
    setProductIcon(QIcon(":/resources/image/logo/logo.svg"));
    setProductName(DApplication::translate("MainWindow", "Deepin Reader"));

//    //  帮助文档
    setApplicationAcknowledgementPage("https://www.deepin.org/acknowledgments/deepin_reader/");

    DApplicationSettings savetheme;

    installEventFilter(new GlobalEventFilter);
    setAboutDialog(new AppAboutWidget);

    initChildren();
}

void Application::handleQuitAction()
{
    MsgSubject::getInstance()->sendMsg(nullptr, MSG_OPERATION_EXIT, "");
}

void Application::initChildren()
{
    dbM = DBManager::instance();
//   setter = ConfigSetter::instance();
}

void Application::initI18n()
{
    // install translators
//    loadTranslator(QList<QLocale>() << QLocale::system());
}
