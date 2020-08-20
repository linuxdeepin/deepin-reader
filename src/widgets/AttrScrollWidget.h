#ifndef ATTRSCROLLWIDGET_H
#define ATTRSCROLLWIDGET_H

#include <DFrame>
#include <DWidget>

class DocSheet;
class QGridLayout;
class AttrScrollWidget : public Dtk::Widget::DFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(AttrScrollWidget)

public:
    explicit AttrScrollWidget(DocSheet *sheet, Dtk::Widget::DWidget *parent  = nullptr);

private:
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const QString &sData);
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const QDateTime &sData);
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const bool &bData);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

    QList<QWidget *> m_leftWidgetlst;
    QList<QWidget *> m_rightWidgetlst;
};

#endif // ATTRSCROLLWIDGET_H
