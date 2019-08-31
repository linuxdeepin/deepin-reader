#include "MainWidget.h"
#include <DDesktopServices>
#include <QDebug>

#include "data/DataManager.h"
#include "FileAttrWidget.h"

MainWidget::MainWidget(DWidget *parent):
    DWidget (parent)
{
    m_centralLayout = new QVBoxLayout();

    // Init layout and editor.
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(0);

    this->setLayout(m_centralLayout);

    initWidgets();

    m_pMsgSubject = MsgSubject::getInstance();
    if(m_pMsgSubject)
    {
        m_pMsgSubject->addObserver(this);
    }
}

MainWidget::~MainWidget()
{
    if(m_pMsgSubject)
    {
        m_pMsgSubject->removeObserver(this);
    }

    if(m_pAttrWidget)
    {
        m_pAttrWidget->deleteLater();
        m_pAttrWidget = nullptr;
    }
}

//  open file
void MainWidget::showFileSelected(const QStringList files) const
{
    int nSize = files.count();
    QString filePath = files.at(0);

    //  判断文件 是否有损坏

    DataManager::instance()->setStrOnlyFilePath(filePath);
    //  open  file
    m_pStackedWidget->setCurrentIndex(1);
}

 //  打开文件所在文件夹
void MainWidget::slotOpenFileFolder()
{
    QString strFilePath = DataManager::instance()->strOnlyFilePath();
    if(strFilePath != "")
    {
        int nLastPos = strFilePath.lastIndexOf('/');
        strFilePath = strFilePath.mid(0, nLastPos);
        DDesktopServices::showFolder(strFilePath);
    }
}

//  查看 文件属性
void MainWidget::slotFileAttr()
{
    if(m_pAttrWidget == nullptr)
    {
       m_pAttrWidget = new FileAttrWidget();
    }
    m_pAttrWidget->show();
}

void MainWidget::initWidgets()
{
    m_pStackedWidget = new DStackedWidget;
    m_centralLayout->addWidget(m_pStackedWidget);

    m_pHomeWidget = new  HomeWidget;
    connect(m_pHomeWidget, SIGNAL(fileSelected(const QStringList)), this, SLOT(showFileSelected(const QStringList)));
    connect(this, SIGNAL(sigHomeOpenFile()), m_pHomeWidget, SLOT(onChooseBtnClicked()));
    m_pStackedWidget->addWidget(m_pHomeWidget);

    m_pMainShowSplitter = new MainShowSplitter;
    connect(this, SIGNAL(setShowSliderState(const bool&)), m_pMainShowSplitter, SLOT(setSidebarVisible(const bool&)));
    connect(this, SIGNAL(setHandShapeState(const bool&)), m_pMainShowSplitter, SLOT(setFileHandShapeState(const bool&)));
    connect(this, SIGNAL(setMagnifyingState(const bool&)), m_pMainShowSplitter, SIGNAL(sigFileMagnifyingState(const bool&)));

    m_pStackedWidget->addWidget(m_pMainShowSplitter);

    m_pStackedWidget->setCurrentIndex(0);
}

int MainWidget::update(const int&, const QString &)
{
    return 0;
}


