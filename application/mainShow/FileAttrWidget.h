#ifndef FILEATTRWIDGET_H
#define FILEATTRWIDGET_H

#include <DLabel>
#include <DTextEdit>
#include <QVBoxLayout>
#include <QGridLayout>
#include <DWidget>
#include <DWidgetUtil>

#include "utils/utils.h"

DWIDGET_USE_NAMESPACE
/**
 * @brief The FileAttrWidget class
 * @brief   文件属性框
 */

class FileAttrWidget : public DAbstractDialog
{
    Q_OBJECT
public:
    FileAttrWidget(DWidget *parent = nullptr);

public:
    void showScreenCenter();

private:
    void initWidget();
    void setFileAttr();
    void initCloseBtn();
    void initImageLabel();
#if 0
    QString elideText(const QString &text, const QSizeF &size,
                                 QTextOption::WrapMode wordWrap,
                                 const QFont &font,
                                 Qt::TextElideMode mode,
                                 qreal lineHeight,
                                 qreal flags = 0);

    void elideText(QTextLayout *layout, const QSizeF &size,
                              QTextOption::WrapMode wordWrap,
                              Qt::TextElideMode mode, qreal lineHeight,
                              int flags = 0, QStringList *lines = nullptr,
                              QPainter *painter = nullptr, QPointF offset = QPoint(0, 0),
                              const QColor &shadowColor = QColor(),
                              const QPointF &shadowOffset = QPointF(0, 1),
                              const QBrush &background = QBrush(Qt::NoBrush),
//                              qreal backgroundReaius = 4,
                              QList<QRectF> *boundingRegion = nullptr);
#endif
    QString calcText(const QFont &font, const QString &filename, const QSize &size);
private slots:
    void slotBtnCloseClicked();

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    QVBoxLayout *m_pVBoxLayout = nullptr;

    DLabel *labelImage = nullptr;
    DLabel *labelFileName = nullptr;
    QString m_strFileName = "";     // 文件名
};

#endif // FILEATTRWIDGET_H
