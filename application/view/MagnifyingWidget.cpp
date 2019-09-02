#include "MagnifyingWidget.h"
#include <QPainter>

MagnifyingWidget::MagnifyingWidget(CustomWidget *parent)
    : CustomWidget(parent)
{
    this->setFixedSize(QSize(50, 50));
    this->setVisible(false); //  默认隐藏
}

void MagnifyingWidget::paintEvent(QPaintEvent *event)
{
    DWidget::paintEvent(event);

    QPainter painter(this);
    painter.save();

    QRect rect = this->geometry();
    painter.setBrush(QBrush(QColor(255, 0, 0)));
    painter.drawEllipse(0, 0, rect.height(), rect.width());

    painter.restore();
}

void MagnifyingWidget::initWidget()
{

}

int MagnifyingWidget::update(const int &msgType, const QString &)
{
    if (msgType == MSG_UPDATE_THEME) {  //  更新主题特殊， 即使处理， 也不截断该消息
        return 0;
    }
    return 0;
}
