#ifndef PAGEBASE_H
#define PAGEBASE_H
#include <QObject>
#include <QLabel>

namespace Page {
enum LinkType_EM {
    LinkType_NULL = 0,
    LinkType_Goto,
    LinkType_GotoOtherFile,
    LinkType_Browse,
    LinkType_Execute
};

struct Link {
    QRectF boundary;

    int page;
    qreal left;
    qreal top;

    QString urlOrFileName;

    LinkType_EM type;

    Link() : boundary(), page(-1), left(qQNaN()), top(qQNaN()), urlOrFileName(), type(LinkType_NULL) {}

    Link(const QRectF &boundingRect, int page, qreal left = qQNaN(), qreal top = qQNaN()) : boundary(boundingRect), page(page), left(left), top(top), urlOrFileName(), type(LinkType_Goto) {}

    Link(const QRectF &boundingRect, const QString &url, LinkType_EM type = LinkType_Browse) : boundary(boundingRect), page(-1), left(qQNaN()), top(qQNaN()), urlOrFileName(url), type(type) {}

    Link(const QRectF &boundingRect, const QString &fileName, int page) : boundary(boundingRect), page(page), left(qQNaN()), top(qQNaN()), urlOrFileName(fileName), type(LinkType_GotoOtherFile) {}

};
}
enum RotateType_EM {
    RotateType_Normal = 0,
    RotateType_90,
    RotateType_180,
    RotateType_270
};

class PageBase: public QLabel
{
    Q_OBJECT
public:
    PageBase(QWidget *parent = 0);
protected:
    RotateType_EM m_rotate;
    double m_scale;
};

#endif // PAGEBASE_H
