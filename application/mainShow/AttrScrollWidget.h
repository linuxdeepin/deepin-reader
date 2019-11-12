#ifndef ATTRSCROLLWIDGET_H
#define ATTRSCROLLWIDGET_H

#include <DScrollArea>
#include <DLabel>
#include <QGridLayout>

DWIDGET_USE_NAMESPACE

/**
 * @brief The AttrScrollWidget class
 * @brief   属性显示
 */


class AttrScrollWidget : public DScrollArea
{
    Q_OBJECT
public:
    AttrScrollWidget(DWidget *parent  = nullptr);

private:
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const QString &sData);
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const QDateTime &sData);
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const bool &bData);
    QString getTime(const QDateTime &CreateTime);
};

#endif // ATTRSCROLLWIDGET_H
