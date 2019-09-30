#include "HomeWidget.h"
#include <QDir>
#include <DFileDialog>
#include <QDebug>
#include <QMimeData>
#include <DPushButton>
#include <QVBoxLayout>
#include <DLabel>
#include "translator/Frame.h"

HomeWidget::HomeWidget(CustomWidget *parent):
    CustomWidget ("HomeWidget", parent),
    m_settings(new QSettings(QDir(Utils::getConfigPath()).filePath("config.conf"),
                             QSettings::IniFormat))
{
    setAcceptDrops(true);
    initWidget();

    // initalize the configuration file.
    if (m_settings->value("dir").toString().isEmpty()) {
        m_settings->setValue("dir", "");
    }
}

void HomeWidget::initWidget()
{
    DLabel *iconLabel = new DLabel;
    iconLabel->setPixmap(QPixmap(":/resources/import_photo.svg"));

    DLabel *tipsLabel = new DLabel(Frame::sDragFile);
    QPalette pe;
    pe.setColor(QPalette::WindowText, QColor("#7a7a7a"));
    tipsLabel->setPalette(pe);

    DLabel *splitLine = new DLabel;
    splitLine->setPixmap(QPixmap(":/images/split_line.svg"));

    auto chooseBtn  = new DPushButton(Frame::sSelectFile);
    chooseBtn->setFixedSize(QSize(302, 36));
    connect(chooseBtn, &DPushButton::clicked, this, &HomeWidget::slotChooseBtnClicked);

    connect(this, SIGNAL(sigOpenFileDialog()), this, SLOT(slotChooseBtnClicked()));

    auto layout = new QVBoxLayout;
    layout->setSpacing(8);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addStretch(1);

    layout->addWidget(iconLabel, 0, Qt::AlignCenter);
    layout->addWidget(tipsLabel, 0, Qt::AlignHCenter);
    layout->addWidget(splitLine, 0, Qt::AlignHCenter);
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
    if (msgType == MSG_OPERATION_OPEN_FILE) {
        emit sigOpenFileDialog();
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == "Ctrl+O") {   //  Ctrl+O 打开文档
            emit sigOpenFileDialog();
            return ConstantMsg::g_effective_res;
        }
    }
    return 0;
}

//文件拖拽
void HomeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept drag event if mime type is url.
    event->accept();
}

void HomeWidget::dropEvent(QDropEvent *event)
{
    auto mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        foreach (QUrl url, mimeData->urls()) {
            QString sFilePath =  url.toLocalFile();
            QFileInfo info(sFilePath);
            QString sCompleteSuffix = info.completeSuffix();
            if (sCompleteSuffix == "pdf" || sCompleteSuffix == "tiff") {
                //  默认打开第一个
                QString sRes = sFilePath + Constant::sQStringSep;

                sendMsg(MSG_OPEN_FILE_PATH, sRes);
                break;
            }
        }
    }
}
