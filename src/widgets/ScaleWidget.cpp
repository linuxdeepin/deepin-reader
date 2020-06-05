#include "ScaleWidget.h"
#include "DocSheet.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <DIconButton>
#include <DComboBox>

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

    m_scaleComboBox = new DComboBox();
    QFont font = m_scaleComboBox->font();
    font.setPixelSize(14);

    m_scaleComboBox->setFont(font);
    m_scaleComboBox->setInsertPolicy(QComboBox::NoInsert);
    m_scaleComboBox->setDuplicatesEnabled(false); //  重复项 不允许添加
    int tW = 120;
    int tH = 0;

    m_scaleComboBox->setFixedWidth(tW);
    m_scaleComboBox->setEditable(true);
    connect(m_scaleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onReturnPressed()));

    QLineEdit *edit = m_scaleComboBox->lineEdit();
    connect(edit, SIGNAL(returnPressed()), SLOT(onReturnPressed()));
    connect(edit, SIGNAL(editingFinished()), SLOT(onEditFinished()));

    tW = 24;
    tH = 24;
    DIconButton *pPreBtn = new DIconButton(DStyle::SP_DecreaseElement);
    DStyle::setFrameRadius(pPreBtn, 12);
    pPreBtn->setFixedSize(QSize(tW, tH));
    connect(pPreBtn, SIGNAL(clicked()), SLOT(slotPrevScale()));

    DIconButton *pNextBtn = new DIconButton(DStyle::SP_IncreaseElement);
    DStyle::setFrameRadius(pNextBtn, 12);
    pNextBtn->setFixedSize(QSize(tW, tH));
    connect(pNextBtn, SIGNAL(clicked()), SLOT(slotNextScale()));

    m_layout->addWidget(pPreBtn);
    m_layout->addWidget(m_scaleComboBox);
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

    QString text = m_scaleComboBox->currentText();

    double value = text.replace("%", "").toDouble() / 100.00;

    if (value <= 0.1)
        value = 0.1;

    if (value > 12)
        value = 12;

    m_sheet->setScaleFactor(value);
}

void ScaleWidget::onEditFinished()
{
    QString text = QString::number(QString::number(m_sheet->operation().scaleFactor * 100, 'f', 2).toDouble()) + "%";

    m_scaleComboBox->setCurrentText(text);
}

void ScaleWidget::setSheet(DocSheet *sheet)
{
    m_sheet = sheet;

    if (nullptr == sheet)
        return;

    m_scaleComboBox->blockSignals(true);

    m_scaleComboBox->clear();

    for (int i = 0; i < m_sheet->scaleFactorList().count(); ++i) {
        m_scaleComboBox->addItem(QString::number(m_sheet->scaleFactorList().at(i) * 100) + "%");
    }

    int index = m_sheet->scaleFactorList().indexOf(m_sheet->operation().scaleFactor);

    m_scaleComboBox->setCurrentIndex(index);

    QString text = QString::number(QString::number(m_sheet->operation().scaleFactor * 100, 'f', 2).toDouble()) + "%";

    m_scaleComboBox->setCurrentText(text);

    m_scaleComboBox->lineEdit()->setCursorPosition(0);

    m_scaleComboBox->blockSignals(false);
}

void ScaleWidget::clear()
{
    if (m_scaleComboBox) {
        m_scaleComboBox->lineEdit()->clear();
    }
}

void ScaleWidget::paintEvent(QPaintEvent *event)
{
    return;
}
