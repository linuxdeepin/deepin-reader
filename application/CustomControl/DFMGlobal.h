#ifndef DFMGLOBAL_H
#define DFMGLOBAL_H

#include <QTextLayout>

class DFMGlobal : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DFMGlobal)

public:
    static DFMGlobal *instance();

protected:
    DFMGlobal();

public:
    static QString elideText(const QString &text, const QSizeF &size,
                             QTextOption::WrapMode wordWrap,
                             const QFont &font,
                             Qt::TextElideMode mode,
                             qreal lineHeight,
                             qreal flags = 0);

private:
    static void elideText(QTextLayout *layout, const QSizeF &size,
                          QTextOption::WrapMode wordWrap,
                          Qt::TextElideMode mode, qreal lineHeight,
                          int flags = 0, QStringList *lines = nullptr,
                          QPainter *painter = nullptr, QPointF offset = QPoint(0, 0),
                          const QColor &shadowColor = QColor(),
                          const QPointF &shadowOffset = QPointF(0, 1),
                          const QBrush &background = QBrush(Qt::NoBrush),
                          qreal backgroundReaius = 4,
                          QList<QRectF> *boundingRegion = nullptr);

};

#endif // DFMGLOBAL_H
