#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <DWidget>
#include <QHBoxLayout>
#include <DPushButton>

DWIDGET_USE_NAMESPACE


/**
 * @brief The TitleWidget class
 * @brief   标题栏的 按钮操作
 */



class TitleWidget : public DWidget
{
    Q_OBJECT
public:
    TitleWidget(DWidget *parent = nullptr);

signals:
    void sendTitleBtnState(const int&) const;
    void sendThumbnailState(const bool&) const;

private slots:
     void on_thumbnailBtn_clicked(bool);
    void on_magnifyingBtn_clicked(bool);

private:
    void initBtns();
    void createBtn(const QString& iconName, const char* member, bool checkable = false, bool checked = false);

private:
    QHBoxLayout *m_layout = nullptr;
};

#endif // TITLEWIDGET_H
