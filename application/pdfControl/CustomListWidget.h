#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <DListWidget>

DWIDGET_USE_NAMESPACE

/**
 * @brief The CustomListWidget class
 * 自定义缩略图ListWidget
 */
class CustomListWidget : public DListWidget
{
    Q_OBJECT
public:
    CustomListWidget(DWidget *parent = nullptr);

signals:
    void sigSelectItem(QListWidgetItem *);

public:
    QListWidgetItem *insertWidgetItem(const int &);

private slots:
    void slot_loadImage(const int &row, const QImage &image);
    void slotShowSelectItem(QListWidgetItem *);
};

#endif // CUSTOMLISTWIDGET_H
