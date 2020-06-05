/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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
#include "CentralNavPage.h"
#include "MainWindow.h"
#include "CustomControl/CustomClickLabel.h"
#include "app/ProcessController.h"

#include <DFileDialog>
#include <DLabel>
#include <DPushButton>
#include <DSuggestButton>
#include <QVBoxLayout>
#include <QSvgWidget>

CentralNavPage::CentralNavPage(DWidget *parent)
    : CustomWidget(parent)
{
    initWidget();
    slotUpdateTheme();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &CentralNavPage::slotUpdateTheme);
}

CentralNavPage::~CentralNavPage()
{

}

void CentralNavPage::initWidget()
{
    auto tipsLabel = new CustomClickLabel(tr("Drag documents here"), this);
    tipsLabel->setAlignment(Qt::AlignHCenter);
    tipsLabel->setForegroundRole(DPalette::TextTips);
    DFontSizeManager::instance()->bind(tipsLabel, DFontSizeManager::T8);

    auto formatLabel = new CustomClickLabel(tr("Format supported: %1").arg("PDF,DJVU"), this);
    formatLabel->setAlignment(Qt::AlignHCenter);
    formatLabel->setForegroundRole(DPalette::TextTips);
    DFontSizeManager::instance()->bind(formatLabel, DFontSizeManager::T8);

    auto chooseBtn = new DSuggestButton(tr("Select File"), this);
    chooseBtn->setFocusPolicy(Qt::StrongFocus);
    int tW = 302;
    int tH = 36;
    chooseBtn->setFixedSize(QSize(tW, tH));
    connect(chooseBtn, &DPushButton::clicked, this, &CentralNavPage::slotChooseBtnClicked);

    auto layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);
    layout->addStretch();

    auto iconSvg = new DLabel(this);
    tW = 128;
    tH = 128;
    iconSvg->setFixedSize(QSize(tW, tW));
    iconSvg->setObjectName("iconSvg");

    layout->addWidget(iconSvg, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(tipsLabel);
    layout->addWidget(formatLabel);
    layout->addSpacing(14);
    layout->addWidget(chooseBtn, 1, Qt::AlignHCenter);
    layout->addStretch();

    this->setLayout(layout);
}

void CentralNavPage::slotChooseBtnClicked()
{
    emit sigNeedOpenFilesExec();
}

//  主题切换
void CentralNavPage::slotUpdateTheme()
{
    auto iconSvg = this->findChild<DLabel *>("iconSvg");
    if (iconSvg) {
        auto plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
        plt.setColor(Dtk::Gui::DPalette::Background, plt.color(Dtk::Gui::DPalette::Window/*Base*/));
        iconSvg->setPalette(plt);
//        QString sPixmap = PF::getImagePath("import_photo", Pri::g_actions);
        QIcon importIcon = /*PF::getIcon*/QIcon::fromTheme(Pri::g_module + "import_photo");
        int tW = 128;
        int tH = 128;

        iconSvg->setPixmap(importIcon.pixmap(QSize(tW, tH)));//Utils::renderSVG(sPixmap, QSize(tW, tH)));
    }

    auto customClickLabelList = this->findChildren<CustomClickLabel *>();
    foreach (auto l, customClickLabelList) {
        l->setForegroundRole(DPalette::TextTips);
    }
}

