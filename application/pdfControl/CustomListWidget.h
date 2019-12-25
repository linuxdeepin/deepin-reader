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
    Q_DISABLE_COPY(CustomListWidget)

public:
    explicit CustomListWidget(DWidget *parent = nullptr);

signals:
    void sigSelectItem(QListWidgetItem *);
    void sigValueChanged(int);

public:
    QListWidgetItem *insertWidgetItem(const int &);
//    inline void setRotateAngele(const int &angle) {m_nRotate = angle;}

private slots:
    void slot_loadImage(const int &row, const QImage &image);
    void slotShowSelectItem(QListWidgetItem *);

//private:
//    int m_nRotate = 0;           // 缩略图旋转角度
};

#endif // CUSTOMLISTWIDGET_H
