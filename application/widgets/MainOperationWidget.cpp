#include "MainOperationWidget.h"
#include <QDebug>

MainOperationWidget::MainOperationWidget(DWidget* parent):
    DWidget (parent)
{
    m_hboxLayout = new QHBoxLayout;
    m_hboxLayout->setContentsMargins(0, 0, 0, 0);
    m_hboxLayout->setSpacing(0);

    this->setLayout(m_hboxLayout);

    initBtns();

    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }
}

MainOperationWidget::~MainOperationWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

void MainOperationWidget::initBtns()
{
    createBtn(tr("缩略图"), SLOT(on_thumbnailBtn_clicked(bool)), true);
    createBtn(tr("书签"), SLOT(on_bookmarksBtn_clicked(bool)), true);
    createBtn(tr("注释"), SLOT(on_annotationBtn_clicked(bool)), true);
}

void MainOperationWidget::createBtn(const QString& iconName, const char* member, bool checkable, bool checked)
{
    DPushButton * btn = new DPushButton(iconName);
    btn->setToolTip(iconName);
    if(checkable)
    {
        btn->setCheckable(true);
        btn->setChecked(checked);

        connect(btn, SIGNAL(clicked(bool)), member);
    }
    else
    {
        connect(btn, SIGNAL(clicked()), member);
    }

    m_hboxLayout->addWidget(btn);
}

void MainOperationWidget::on_thumbnailBtn_clicked(bool)
{
 //   qDebug() << "showType(0)";
    emit showType(0);
}

void MainOperationWidget::on_bookmarksBtn_clicked(bool)
{
 //   qDebug() << "showType(1)";
    emit showType(1);
}

void MainOperationWidget::on_annotationBtn_clicked(bool)
{
//    qDebug() << "showType(2)";
    emit showType(2);
}

int MainOperationWidget::update(const QString &)
{
    return 0;
}
