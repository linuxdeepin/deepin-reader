#include "docummentfactory.h"
#include "pdf/docummentpdf.h"

DocummentFactory::DocummentFactory()
{

}

DocummentBase *DocummentFactory::creatDocumment(DocType_EM type, QWidget *father)
{
    DocummentBase *documment;
    switch (type) {
    case DocType_PDF:
        documment = new DocummentPDF(father);
        break;
    default:
        break;
    }
    return documment;
}
