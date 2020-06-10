#ifndef SCALEWIDGET_H
#define SCALEWIDGET_H

#include "CustomControl/CustomWidget.h"
#include <QPointer>

class DocSheet;
class ScaleWidget : public DWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ScaleWidget)
public:
    explicit ScaleWidget(DWidget *parent = nullptr);

    ~ScaleWidget() override;

    void setSheet(DocSheet *sheet);

    void clear();

    void paintEvent(QPaintEvent *event) override;

protected:
    void initWidget();

private slots:
    void slotPrevScale();

    void slotNextScale();

    void onReturnPressed();

    void onEditFinished();

private:
    DComboBox  *m_scaleComboBox = nullptr;
    QPointer<DocSheet> m_sheet;
};

#endif // SCALEWIDGET_H
