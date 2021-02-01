#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <DDialog>

class QProgressBar;
class ProgressDialog : public Dtk::Widget::DDialog
{
    Q_OBJECT
public:
    explicit ProgressDialog(QWidget *parent = nullptr);

private:
    QProgressBar *m_progress = nullptr;
};

#endif // PROGRESSDIALOG_H
