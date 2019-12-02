#ifndef OBJECTEVENTFILTER_H
#define OBJECTEVENTFILTER_H

#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QWheelEvent>

class ObjectEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit ObjectEventFilter(QObject *parent = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;

private:
    void dealWithKeyEvent(const QString &key);
    void onOpenAppHelp();
    void displayShortcuts();

    void notifyMsg(const int &, const QString &msgContent = "");

private:
    QStringList m_pFilterList;
};

#endif // OBJECTEVENTFILTER_H
