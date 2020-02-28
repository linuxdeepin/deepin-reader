#include "HomeWidget.h"

#include <DFileDialog>
#include <DLabel>
#include <DPushButton>
#include <DSuggestButton>
#include <QVBoxLayout>
#include "CustomControl/CustomClickLabel.h"
#include "utils/PublicFunction.h"

#include <QSvgWidget>

HomeWidget::HomeWidget(CustomWidget *parent)
    : CustomWidget("HomeWidget", parent)
{
    m_pMsgList = {MSG_MENU_NEW_WINDOW};
    m_pKeyMsgList = {KeyStr::g_ctrl_o};

    initWidget();
    initConnections();

    slotUpdateTheme();

    dApp->m_pModelService->addObserver(this);
}

HomeWidget::~HomeWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

void HomeWidget::slotDealWithData(const int &msgType, const QString &)
{
    if (MSG_MENU_NEW_WINDOW == msgType) {
        NewWindow();
    }
}

void HomeWidget::slotDealWithKeyMsg(const QString &msgContent)
{
    if (msgContent == KeyStr::g_ctrl_o) {
        slotChooseBtnClicked();
    }
}

void HomeWidget::initWidget()
{
    auto tipsLabel = new CustomClickLabel(tr("Drag PDF files here"), this);
    tipsLabel->setAlignment(Qt::AlignHCenter);
    tipsLabel->setForegroundRole(DPalette::TextTips);
    DFontSizeManager::instance()->bind(tipsLabel, DFontSizeManager::T8);

    auto chooseBtn = new DSuggestButton(tr("Select File"));
    chooseBtn->setFixedSize(QSize(302, 36));
    connect(chooseBtn, &DPushButton::clicked, this, &HomeWidget::slotChooseBtnClicked);

    auto layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);
    layout->addStretch();

    auto iconSvg = new DLabel(this);
    iconSvg->setFixedSize(QSize(128, 128));
    iconSvg->setObjectName("iconSvg");

    layout->addWidget(iconSvg, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(tipsLabel);
    layout->addSpacing(14);
    layout->addWidget(chooseBtn, 1, Qt::AlignHCenter);
    layout->addStretch();

    this->setLayout(layout);
}

void HomeWidget::slotChooseBtnClicked()
{
    QStringList fileList = getOpenFileList();
    if (fileList.size() > 0) {
        QString sRes = "";

        foreach (auto s, fileList) {
            sRes += s + Constant::sQStringSep;
        }

        emit sigOpenFilePaths(sRes);
    }
}

//  主题切换
void HomeWidget::slotUpdateTheme()
{
    auto iconSvg = this->findChild<DLabel *>("iconSvg");
    if (iconSvg) {
        auto plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
        plt.setColor(Dtk::Gui::DPalette::Background, plt.color(Dtk::Gui::DPalette::Base));
        iconSvg->setPalette(plt);
        QString sPixmap = PF::getImagePath("import_photo", Pri::g_actions);
        iconSvg->setPixmap(Utils::renderSVG(sPixmap, QSize(128, 128)));
    }

    auto customClickLabelList = this->findChildren<CustomClickLabel *>();
    foreach (auto l, customClickLabelList) {
        l->setForegroundRole(DPalette::TextTips);
    }
}

QStringList HomeWidget::getOpenFileList()
{
    QStringList fileList;

    DFileDialog dialog;
    dialog.setFileMode(DFileDialog::ExistingFiles);
    dialog.setNameFilter(Utils::getSuffixList());

    QString historyDir = QDir::homePath();
    dialog.setDirectory(historyDir);

    const int mode = dialog.exec();

    // if click cancel button or close button.
    if (mode != QDialog::Accepted) {
        return fileList;
    }
    fileList = dialog.selectedFiles();
    return fileList;
}

void HomeWidget::initConnections()
{
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
    connect(this, SIGNAL(sigDealWithKeyMsg(const QString &)), SLOT(slotDealWithKeyMsg(const QString &)));
}

void HomeWidget::NewWindow()
{

}

int HomeWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (m_pKeyMsgList.contains(msgContent)) {
            emit sigDealWithKeyMsg(msgContent);
            return ConstantMsg::g_effective_res;
        }
    }

    return 0;
}
