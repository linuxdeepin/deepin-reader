#include "ScaleWidget.h"
#include "DocSheet.h"
#include "ScaleMenu.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <DIconButton>

ScaleWidget::ScaleWidget(DWidget *parent)
    : DWidget(parent)
{
    initWidget();
}

ScaleWidget::~ScaleWidget()
{

}

void ScaleWidget::initWidget()
{
    auto m_layout = new QHBoxLayout();
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(10);
    setLayout(m_layout);

    m_lineEdit = new DLineEdit(this);
    QFont font = m_lineEdit->font();
    font.setPixelSize(14);
    m_lineEdit->setFont(font);
    m_lineEdit->setFixedSize(120, 36);

    DIconButton *arrowBtn = new DIconButton(QStyle::SP_ArrowDown, m_lineEdit);
    arrowBtn->setFixedSize(32, 32);
    arrowBtn->move(m_lineEdit->width() - arrowBtn->width() - 2, 2);
    m_lineEdit->lineEdit()->setTextMargins(0, 0, arrowBtn->width(), 0);
    m_lineEdit->setClearButtonEnabled(false);

    connect(m_lineEdit, SIGNAL(returnPressed()), SLOT(onReturnPressed()));
    connect(m_lineEdit, SIGNAL(editingFinished()), SLOT(onEditFinished()));
    connect(arrowBtn, SIGNAL(clicked()), SLOT(onArrowBtnlicked()));

    DIconButton *pPreBtn = new DIconButton(DStyle::SP_DecreaseElement);
    DStyle::setFrameRadius(pPreBtn, 12);
    pPreBtn->setFixedSize(QSize(24, 24));
    connect(pPreBtn, SIGNAL(clicked()), SLOT(slotPrevScale()));

    DIconButton *pNextBtn = new DIconButton(DStyle::SP_IncreaseElement);
    DStyle::setFrameRadius(pNextBtn, 12);
    pNextBtn->setFixedSize(QSize(24, 24));
    connect(pNextBtn, SIGNAL(clicked()), SLOT(slotNextScale()));

    m_layout->addWidget(pPreBtn);
    m_layout->addWidget(m_lineEdit);
    m_layout->addWidget(pNextBtn);
}

void ScaleWidget::slotPrevScale()
{
    if (m_sheet.isNull())
        return;

    m_sheet->zoomout();
}

void ScaleWidget::slotNextScale()
{
    if (m_sheet.isNull())
        return;

    m_sheet->zoomin();
}

void ScaleWidget::onReturnPressed()
{
    if (m_sheet.isNull())
        return;

    qreal value = m_lineEdit->text().replace("%", "").toDouble() / 100.00;
    value = qBound(0.1, value, m_sheet->scaleFactorList().last());
    m_sheet->setScaleFactor(value);
}

void ScaleWidget::onArrowBtnlicked()
{
    m_lineEdit->lineEdit()->setFocus(Qt::MouseFocusReason);

    ScaleMenu scaleMenu;
    QPoint point = m_lineEdit->mapToGlobal(QPoint(0, m_lineEdit->height() + 2));
    scaleMenu.readCurDocParam(m_sheet.data());
    scaleMenu.exec(point);
}

void ScaleWidget::onEditFinished()
{
    QString text = QString::number(QString::number(m_sheet->operation().scaleFactor * 100, 'f', 2).toDouble()) + "%";
    m_lineEdit->setText(text);
}

void ScaleWidget::setSheet(DocSheet *sheet)
{
    m_sheet = sheet;

    if (nullptr == sheet)
        return;

    m_lineEdit->clear();
    QString text = QString::number(QString::number(m_sheet->operation().scaleFactor * 100, 'f', 2).toDouble()) + "%";
    m_lineEdit->setText(text);
    m_lineEdit->lineEdit()->setCursorPosition(0);
}

void ScaleWidget::clear()
{
    m_lineEdit->clear();
}

void ScaleWidget::paintEvent(QPaintEvent *)
{
    return;
}
