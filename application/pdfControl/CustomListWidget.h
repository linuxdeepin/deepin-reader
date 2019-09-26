#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <DListWidget>

DWIDGET_USE_NAMESPACE

class CustomListWidget : public DListWidget
{
    Q_OBJECT
public:
    CustomListWidget(DWidget *parent = nullptr);

public:
    void setItemImage(const int &row, QImage &image);

private slots:
    void slotShowSelectItem(QListWidgetItem *);
};

#endif // CUSTOMLISTWIDGET_H
