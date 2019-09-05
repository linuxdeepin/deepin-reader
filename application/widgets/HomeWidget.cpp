#include "HomeWidget.h"
#include <QApplication>
#include <QDir>
#include <DFileDialog>

HomeWidget::HomeWidget(CustomWidget *parent):
    CustomWidget ("HomeWidget", parent),
    m_layout(new QVBoxLayout(this)),
    m_settings(new QSettings(QDir(Utils::getConfigPath()).filePath("config.conf"),
                             QSettings::IniFormat))
{
    initWidget();

    // initalize the configuration file.
    if (m_settings->value("dir").toString().isEmpty()) {
        m_settings->setValue("dir", "");
    }
}

void HomeWidget::initWidget()
{
    DLabel *iconLabel = new DLabel;
    iconLabel->setPixmap(QPixmap(":/image/icon_import_photo.svg"));

    DLabel *tipsLabel = new DLabel(tr("drag font file here"));
    tipsLabel->setStyleSheet("QLabel { color: #6a6a6a; }");

    DLabel *splitLine = new DLabel;
    splitLine->setPixmap(QPixmap(":/images/split_line.svg"));

    DPushButton *chooseBtn  = new DPushButton(tr("select file"));
    chooseBtn->setFixedSize(QSize(302, 36));
    connect(chooseBtn, &DPushButton::clicked, this, &HomeWidget::onChooseBtnClicked);

    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addStretch(1);
    m_layout->addWidget(iconLabel, 0, Qt::AlignCenter);
    m_layout->addWidget(tipsLabel, 0, Qt::AlignHCenter);
    m_layout->addWidget(splitLine, 0, Qt::AlignHCenter);
    m_layout->addWidget(chooseBtn, 0, Qt::AlignHCenter);
    m_layout->addStretch(1);
}

void HomeWidget::onChooseBtnClicked()
{
    QStringList fileList = getOpenFileList();
    if (fileList.size() > 0) {
        emit fileSelected(fileList);
    }
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


int HomeWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    Q_UNUSED(msgContent);
    if (msgType == MSG_OPERATION_OPEN_FILE) {
        onChooseBtnClicked();
        return ConstantMsg::g_effective_res;
    }
    return 0;
}
