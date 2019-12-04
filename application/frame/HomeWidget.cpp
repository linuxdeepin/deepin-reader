#include "HomeWidget.h"

#include <DFileDialog>
#include <DPushButton>
#include <DSuggestButton>
#include <QVBoxLayout>
#include <DLabel>
#include "CustomControl/CustomClickLabel.h"
#include "utils/PublicFunction.h"

#include <QSvgWidget>

HomeWidget::HomeWidget(CustomWidget *parent):
    CustomWidget("HomeWidget", parent)
{
    initWidget();
    initConnections();

    m_settings = AppSetting::instance();

    slotUpdateTheme();
}

void HomeWidget::initWidget()
{
    auto tipsLabel = new CustomClickLabel(tr("drag Pdf or other format file to here"), this);
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

    auto iconSvg = new DLabel;
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

        notifyMsg(MSG_OPEN_FILE_PATH_S, sRes);
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
        QString sPixmap = PF::getImagePath("import_photo", Pri::g_frame);
        iconSvg->setPixmap(Utils::renderSVG(sPixmap, QSize(128, 128)));
    }

    auto customClickLabelList = this->findChildren<CustomClickLabel *>();
    foreach (auto l, customClickLabelList) {
        l->setThemePalette();
    }
}

QStringList HomeWidget::getOpenFileList()
{
    QStringList fileList;

    DFileDialog dialog;
    dialog.setFileMode(DFileDialog::ExistingFiles);
    dialog.setNameFilter(Utils::getSuffixList());

    QString historyDir = m_settings->getKeyValue(KEY_DIR);
    if (historyDir.isEmpty()) {
        historyDir = QDir::homePath();
    }
    dialog.setDirectory(historyDir);

    const int mode = dialog.exec();

    // save the directory string to config file.
    m_settings->setFileKeyValue(dialog.directoryUrl().toLocalFile());

    // if click cancel button or close button.
    if (mode != QDialog::Accepted) {
        return fileList;
    }
    fileList = dialog.selectedFiles();
    return fileList;
}

void HomeWidget::initConnections()
{
    connect(this, SIGNAL(sigOpenFileDialog()), SLOT(slotChooseBtnClicked()));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
}

int HomeWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == KeyStr::g_ctrl_o) {   //  Ctrl+O 打开文档
            emit sigOpenFileDialog();
            return ConstantMsg::g_effective_res;
        }
    }

    if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }
    return 0;
}
