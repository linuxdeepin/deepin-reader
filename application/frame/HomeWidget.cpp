#include "HomeWidget.h"
#include <DGuiApplicationHelper>
#include <DPalette>
#include <DFileDialog>
#include <DPushButton>
#include <DSuggestButton>
#include <QVBoxLayout>
#include <DLabel>
#include <DFontSizeManager>
#include "CustomControl/CustomClickLabel.h"
#include "utils/PublicFunction.h"

#include <QSvgWidget>
#include <DFontSizeManager>

HomeWidget::HomeWidget(CustomWidget *parent):
    CustomWidget("HomeWidget", parent)
{
    initWidget();
    initConnections();

    m_settings = AppSetting::instance();
    // initalize the configuration file.
    m_settings->setKeyValue("dir", "");

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

    auto iconSvg = new QSvgWidget;
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

        foreach (const QString &s, fileList) {
            sRes += s + Constant::sQStringSep;
        }
        notifyMsg(MSG_OPEN_FILE_PATH, sRes);
    }
}

//  主题切换
void HomeWidget::slotUpdateTheme()
{
    auto iconSvg = this->findChild<QSvgWidget *>("iconSvg");
    if (iconSvg) {
        QString sPixmap = PF::getImagePath("import_photo", Pri::g_frame);
        iconSvg->load(sPixmap);
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
    dialog.setFileMode(DFileDialog::ExistingFile);
    dialog.setNameFilter(Utils::getSuffixList());

    QString historyDir = m_settings->getKeyValue("dir");
    if (historyDir.isEmpty()) {
        historyDir = QDir::homePath();
    }
    dialog.setDirectory(historyDir);

    const int mode = dialog.exec();

    // save the directory string to config file.
    m_settings->setKeyValue("dir", dialog.directoryUrl().toLocalFile());

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
    if (msgType == MSG_OPERATION_OPEN_FILE) {
        emit sigOpenFileDialog();
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == KeyStr::g_ctrl_o) {   //  Ctrl+O 打开文档
            emit sigOpenFileDialog();
            return ConstantMsg::g_effective_res;
        }
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }
    return 0;
}
