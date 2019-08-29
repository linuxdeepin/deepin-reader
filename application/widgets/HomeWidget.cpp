#include "HomeWidget.h"
#include "utils.h"
#include <QApplication>
#include <QDir>

#include <DFileDialog>


HomeWidget::HomeWidget(DWidget *parent):
    DWidget (parent),
    m_layout(new QVBoxLayout(this)),
    m_iconLabel(new DLabel),
    m_tipsLabel(new DLabel(tr("Drag font file here"))),
    m_splitLine(new DLabel),
    m_chooseBtn(new DLinkButton(tr("Select file"))),
    m_settings(new QSettings(QDir(Utils::getConfigPath()).filePath("config.conf"),
                             QSettings::IniFormat))
{
    m_unloadPixmap = Utils::renderSVG(":/images/font_unload.svg", QSize(160, 160));
    m_loadedPixmap = Utils::renderSVG(":/images/font_loaded.svg", QSize(160, 160));

    m_iconLabel->setFixedSize(160, 160);
    m_iconLabel->setPixmap(m_unloadPixmap);
    m_splitLine->setPixmap(QPixmap(":/images/split_line.svg"));
    m_tipsLabel->setStyleSheet("QLabel { color: #6a6a6a; }");

    // initalize the configuration file.
    if (m_settings->value("dir").toString().isEmpty()) {
        m_settings->setValue("dir", "");
    }

    m_layout->addSpacing(40);
    m_layout->addWidget(m_iconLabel, 0, Qt::AlignTop | Qt::AlignHCenter);
    m_layout->addSpacing(20);
    m_layout->addWidget(m_tipsLabel, 0, Qt::AlignHCenter);
    m_layout->addSpacing(15);
    m_layout->addWidget(m_splitLine, 0, Qt::AlignHCenter);
    m_layout->addSpacing(15);
    m_layout->addWidget(m_chooseBtn, 0, Qt::AlignHCenter);
    m_layout->addStretch();
    m_layout->setSpacing(0);

    connect(m_chooseBtn, &DLinkButton::clicked, this, &HomeWidget::onChooseBtnClicked);

    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }
}

HomeWidget::~HomeWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

void HomeWidget::setIconPixmap(bool isLoaded)
{
    if (isLoaded) {
        m_iconLabel->setPixmap(m_loadedPixmap);
    } else {
        m_iconLabel->setPixmap(m_unloadPixmap);
    }
}

void HomeWidget::onChooseBtnClicked()
{
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
        return;
    }

    emit fileSelected(dialog.selectedFiles());
}

int HomeWidget::update(const QString &)
{
    return 0;
}
