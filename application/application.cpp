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
#include "subjectObserver/ModuleHeader.h"
namespace {

}  // namespace

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    setAttribute(Qt::AA_UseHighDpiPixmaps);
    // setAttribute(Qt::AA_EnableHighDpiScaling);
    // setAttribute(Qt::AA_ForceRasterWidgets);

    initI18n();

    setApplicationName(tr("deepin-reader"));
    setOrganizationName(tr("deepin"));

    setApplicationDisplayName(tr("deepin-reader"));
    setApplicationVersion(DApplication::buildVersion("20191022"));
    setApplicationAcknowledgementPage(Constant::sAcknowledgementLink);
    setProductIcon(QIcon(Constant::sLogoPath));
    setApplicationDescription(tr("Document viewer is a document viewer that comes with the deep operating system.\r\n In"
                                 " addition to opening and reading PDF files, On documents you can also \r\n add"
                                 " bookmark, annotation and highlight selected text."));

    DApplicationSettings savetheme;

    installEventFilter(new GlobalEventFilter);
    //使用这种方式请确保使用dpkg构建后的版本一致（请测试后再使用），否者不要使用
    //setAboutDialog(new AppAboutWidget);

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
    loadTranslator();
}
