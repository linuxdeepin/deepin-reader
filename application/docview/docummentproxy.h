#ifndef DOCUMMENTPROXY_H
#define DOCUMMENTPROXY_H
#include "docummentbase.h"
#include <QObject>
#include <QWidget>

enum DocType_EM {
    DocType_NULL = 0,
    DocType_PDF
};

class DocummentProxy: public QObject
{
    Q_OBJECT
public:
    DocummentProxy(QObject *parent = nullptr);
    bool openFile(DocType_EM type, QString filepath);
    bool mouseSelectText(QPoint start, QPoint stop);
    void mouseSelectTextClear();
    bool mouseBeOverText(QPoint point);
private:
    QWidget *qwfather;
    DocType_EM m_type;
    QString m_path;
    DocummentBase *m_documment;
};

#endif // DOCUMMENTPROXY_H
