#include "CentralNavPage.h"
#include "MainWindow.h"
#include "CustomControl/CustomClickLabel.h"
#include "utils/PublicFunction.h"
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
    auto tipsLabel = new CustomClickLabel(tr("Drag PDF files here"), this);
    tipsLabel->setAlignment(Qt::AlignHCenter);
    tipsLabel->setForegroundRole(DPalette::TextTips);
    DFontSizeManager::instance()->bind(tipsLabel, DFontSizeManager::T8);

    auto chooseBtn = new DSuggestButton(tr("Select File"), this);
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
        QIcon importIcon = PF::getIcon(Pri::g_module + "import_photo");
        int tW = 128;
        int tH = 128;

        iconSvg->setPixmap(importIcon.pixmap(QSize(tW, tH)));//Utils::renderSVG(sPixmap, QSize(tW, tH)));
    }

    auto customClickLabelList = this->findChildren<CustomClickLabel *>();
    foreach (auto l, customClickLabelList) {
        l->setForegroundRole(DPalette::TextTips);
    }
}

