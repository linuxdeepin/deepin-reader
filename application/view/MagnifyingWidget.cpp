#include "MagnifyingWidget.h"
#include <QPainter>

MagnifyingWidget::MagnifyingWidget(CustomWidget *parent)
    : CustomWidget(parent)
{
    this->setFixedSize(QSize(100, 100));
    this->setVisible(false); //  默认隐藏
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    initWidget();
}

//  图形 是 文档传递过来进行展示
void MagnifyingWidget::setShowImage(const QImage &temp)
{
    m_showImage = temp;

    update();
}

void MagnifyingWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();

//    painter.drawImage(100, 100, m_showImage);
    //设置反锯齿
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::Qt4CompatiblePainting);

    painter.setPen(Qt::red);
    painter.setBrush(Qt::red);

    QPainterPath bigCircle;
    bigCircle.addEllipse(0, 0, 100, 100);

    QPainterPath smallCircle;
    smallCircle.addEllipse(10, 10, 80, 80);

    QPainterPath path = bigCircle - smallCircle;
    painter.drawPath(path);

    painter.restore();
}

void MagnifyingWidget::initWidget()
{
}

int MagnifyingWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_UPDATE_THEME) {  //  更新主题特殊， 即使处理， 也不截断该消息
        return 0;
    }
    return 0;
}
