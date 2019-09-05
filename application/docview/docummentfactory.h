#ifndef DOCUMMENTFACTORY_H
#define DOCUMMENTFACTORY_H
#include "docummentbase.h"
#include "docummentproxy.h"

class DocummentFactory
{
public:
    DocummentFactory();

    DocummentBase *creatDocumment(DocType_EM type, QWidget *father);

};

#endif // DOCUMMENTFACTORY_H
