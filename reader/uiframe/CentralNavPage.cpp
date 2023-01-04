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

#include <DLabel>
#include <DPushButton>
#include <DSuggestButton>
#include <DFontSizeManager>
#include <DGuiApplicationHelper>

#include <QVBoxLayout>

CentralNavPage::CentralNavPage(DWidget *parent)
    : BaseWidget(parent)
{
    auto tipsLabel = new DLabel(tr("Drag documents here"), this);
    tipsLabel->setAccessibleName("Label_Drag documents here");
    tipsLabel->setAlignment(Qt::AlignHCenter);
    tipsLabel->setForegroundRole(DPalette::TextTips);
    DFontSizeManager::instance()->bind(tipsLabel, DFontSizeManager::T8);

    auto formatLabel = new DLabel(tr("Format supported: %1").arg("PDF,DJVU,DOCX"), this);
    formatLabel->setAccessibleName(QString("Label_format supported: %1").arg("PDF,DJVU,DOCX"));
    formatLabel->setAlignment(Qt::AlignHCenter);
    formatLabel->setForegroundRole(DPalette::TextTips);
    DFontSizeManager::instance()->bind(formatLabel, DFontSizeManager::T8);

    auto chooseButton = new DSuggestButton(tr("Select File"), this);
    chooseButton->setObjectName("SelectFileBtn");
    chooseButton->setAccessibleName("SelectFile");
    chooseButton->setFocusPolicy(Qt::TabFocus);
    chooseButton->setFixedSize(302, 36);
    connect(chooseButton, &DPushButton::clicked, this, &CentralNavPage::sigNeedOpenFilesExec);

    auto layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);
    layout->addStretch();

    auto iconSvg = new DLabel(this);
    iconSvg->setFixedSize(128, 128);
    iconSvg->setObjectName("iconSvg");
    iconSvg->setAccessibleName("Label_Icon");

    layout->addWidget(iconSvg, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(tipsLabel);
    layout->addWidget(formatLabel);
    layout->addSpacing(14);
    layout->addWidget(chooseButton, 1, Qt::AlignHCenter);
    layout->addStretch();

    this->setLayout(layout);

    onThemeChanged();

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &CentralNavPage::onThemeChanged);
}

//void CentralNavPage::onChooseButtonClicked()
//{
//    emit sigNeedOpenFilesExec();
//}

//  主题切换
void CentralNavPage::onThemeChanged()
{
    auto iconSvg = this->findChild<DLabel *>("iconSvg");
    if (iconSvg) {
        auto plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
        plt.setColor(Dtk::Gui::DPalette::Background, plt.color(Dtk::Gui::DPalette::Window));
        iconSvg->setPalette(plt);
        QIcon importIcon = QIcon::fromTheme(QString("dr_") + "import_photo");
        int tW = 128;
        int tH = 128;

        iconSvg->setPixmap(importIcon.pixmap(QSize(tW, tH)));
    }

    auto customClickLabelList = this->findChildren<DLabel *>();
    foreach (auto l, customClickLabelList) {
        l->setForegroundRole(DPalette::TextTips);
    }
}

