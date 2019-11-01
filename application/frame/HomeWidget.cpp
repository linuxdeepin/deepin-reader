#include "HomeWidget.h"
#include <DFileDialog>
#include <DPushButton>
#include <QVBoxLayout>
#include <DLabel>
#include "utils/PublicFunction.h"

HomeWidget::HomeWidget(CustomWidget *parent):
    CustomWidget ("HomeWidget", parent),
    m_settings(new QSettings(QDir(Utils::getConfigPath()).filePath("config.conf"),
                             QSettings::IniFormat))
{
    initWidget();
    initConnections();

    // initalize the configuration file.
    if (m_settings->value("dir").toString().isEmpty()) {
        m_settings->setValue("dir", "");
    }

    slotUpdateTheme();
}

void HomeWidget::initWidget()
{
    DLabel *tipsLabel = new DLabel(tr("drag Pdf file here"));
    QPalette pe;
    pe.setColor(QPalette::WindowText, QColor("#7a7a7a"));
    tipsLabel->setPalette(pe);

//    DLabel *splitLine = new DLabel;
//    splitLine->setPixmap(QPixmap(":/images/split_line.svg"));

    auto chooseBtn  = new DPushButton(tr("Select File"));
    chooseBtn->setFixedSize(QSize(302, 36));
    connect(chooseBtn, &DPushButton::clicked, this, &HomeWidget::slotChooseBtnClicked);

    connect(this, SIGNAL(sigOpenFileDialog()), this, SLOT(slotChooseBtnClicked()));

    auto layout = new QVBoxLayout;
    layout->setSpacing(8);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addStretch(1);

    m_pIconLabel = new DLabel;
    layout->addWidget(m_pIconLabel, 0, Qt::AlignCenter);

    layout->addWidget(tipsLabel, 0, Qt::AlignHCenter);
//    layout->addWidget(splitLine, 0, Qt::AlignHCenter);
    layout->addWidget(chooseBtn, 0, Qt::AlignHCenter);

    layout->addStretch(1);

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

        sendMsg(MSG_OPEN_FILE_PATH, sRes);
    }
}

//  主题切换
void HomeWidget::slotUpdateTheme()
{
    QString sPixmap = PF::getImagePath("import_photo", Pri::g_frame);
    m_pIconLabel->setPixmap(QPixmap(sPixmap));
}

QStringList HomeWidget::getOpenFileList()
{
    QStringList fileList;

    DFileDialog dialog;
    dialog.setFileMode(DFileDialog::ExistingFile);
    dialog.setNameFilter(Utils::getSuffixList());

    QString historyDir = m_settings->value("dir").toString();
    if (historyDir.isEmpty()) {
        historyDir = QDir::homePath();
    }
    dialog.setDirectory(historyDir);

    const int mode = dialog.exec();

    // save the directory string to config file.
    m_settings->setValue("dir", dialog.directoryUrl().toLocalFile());

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
}

int HomeWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE) {
        emit sigOpenFileDialog();
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == "Ctrl+O") {   //  Ctrl+O 打开文档
            emit sigOpenFileDialog();
            return ConstantMsg::g_effective_res;
        }
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }
    return 0;
}
