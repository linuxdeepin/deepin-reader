#ifndef MAGNIFYINGWIDGET_H
#define MAGNIFYINGWIDGET_H

#include <QPaintEvent>

#include "header/CustomWidget.h"


/**
 * @brief The MagnifyingWidget class
 * @brief   放大镜窗口
 */

class MagnifyingWidget : public CustomWidget
{
    Q_OBJECT
public:
    MagnifyingWidget(CustomWidget *parent = nullptr);

signals:
    void sigDrawImage();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void setShowImage(const QImage &);

private:
    QImage  m_showImage;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

    // CustomWidget interface
protected:
    void initWidget() override;
};

#endif // MAGNIFYINGWIDGET_H
