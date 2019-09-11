#include "HomeWidget.h"
#include <QApplication>
#include <QDir>
#include <DFileDialog>
#include <QDebug>
#include <QMimeData>

HomeWidget::HomeWidget(CustomWidget *parent):
    CustomWidget ("HomeWidget", parent),
    m_layout(new QVBoxLayout(this)),
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

    DLabel *tipsLabel = new DLabel(tr("drag font file here"));
    QPalette pe;
    pe.setColor(QPalette::WindowText, QColor("#7a7a7a"));
    tipsLabel->setPalette(pe);

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

        QString sRes = "";

        foreach (const QString &s, fileList) {
            sRes += s + "@#&wzx";
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
    Q_UNUSED(msgContent);
    if (msgType == MSG_OPERATION_OPEN_FILE) {
        onChooseBtnClicked();
        return ConstantMsg::g_effective_res;
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
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        for (auto url : mimeData->urls()) {
            QString sFilePath =  url.toLocalFile();
            if (sFilePath.endsWith("pdf")) {
                //  默认打开第一个
                QString sRes = sFilePath + "@#&wzx";

                sendMsg(MSG_OPEN_FILE_PATH, sRes);
                break;
            }
        }
    }
}
