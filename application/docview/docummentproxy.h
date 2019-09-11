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
    QPoint global2RelativePoint(QPoint globalpoint);
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0);
    bool mouseSelectText(QPoint start, QPoint stop);
    void mouseSelectTextClear();
    bool mouseBeOverText(QPoint point);
    void scaleRotateAndShow(double scale, RotateType_EM rotate);
    bool getImage(int pagenum, QImage &image, double width, double height);
    int  getPageSNum();
    bool setViewModeAndShow(ViewMode_EM viewmode);
    bool showMagnifier(QPoint point);
    bool setMagnifierStyle(QColor magnifiercolor = Qt::white, int magnifierradius = 100, int magnifierringwidth = 10, double magnifierscale = 3);
private:
    QWidget *qwfather;
    DocType_EM m_type;
    QString m_path;
    DocummentBase *m_documment;
};

#endif // DOCUMMENTPROXY_H
