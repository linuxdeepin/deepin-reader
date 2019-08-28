#include "MainWidget.h"
#include <QDebug>

MainWidget::MainWidget(DWidget *parent):
    DWidget (parent)
{
    m_centralLayout = new QVBoxLayout();

    // Init layout and editor.
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(0);

    this->setLayout(m_centralLayout);

    initWidgets();
}

//  open file
void MainWidget::showFileSelected(const QStringList files) const
{
    int nSize = files.count();
    QString filePath = files.at(0);
    qDebug() << nSize << "    "<< filePath;

    //  open  file
    m_pStackedWidget->setCurrentIndex(1);
}

void MainWidget::setSliderState(const bool &bVis) const
{
    emit setShowSliderState(bVis);
}

void MainWidget::initWidgets()
{
    m_pStackedWidget = new DStackedWidget;
    m_centralLayout->addWidget(m_pStackedWidget);

    m_pHomeWidget = new  HomeWidget;
    connect(m_pHomeWidget, SIGNAL(fileSelected(const QStringList)), this, SLOT(showFileSelected(const QStringList)));
    connect(this, SIGNAL(sigHomeOpenFile()), m_pHomeWidget, SLOT(onChooseBtnClicked()));
    m_pStackedWidget->addWidget(m_pHomeWidget);

    m_pMainShowDataWidget = new MainShowDataWidget;   
    connect(this, SIGNAL(setShowSliderState(const bool&)), m_pMainShowDataWidget, SLOT(setSidebarVisible(const bool&)));

    m_pStackedWidget->addWidget(m_pMainShowDataWidget);

    m_pStackedWidget->setCurrentIndex(0);
}


