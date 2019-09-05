#ifndef DOCUMMENTBASE_H
#define DOCUMMENTBASE_H

#include <QScrollArea>

class DocummentBase: public QScrollArea
{
    Q_OBJECT
public:
    DocummentBase(QWidget *parent = nullptr);
};

#endif // DOCUMMENTBASE_H
