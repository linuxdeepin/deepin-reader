#ifndef PAGEBASE_H
#define PAGEBASE_H
#include <QObject>
#include <QLabel>

class PageBase: public QLabel
{
    Q_OBJECT
public:
    PageBase(QWidget *parent = 0);
};

#endif // PAGEBASE_H
