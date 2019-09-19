#include "docummentfactory.h"
#include "pdf/docummentpdf.h"
#include "djvu/docummentdjvu.h"

DocummentFactory::DocummentFactory()
{

}

DocummentBase *DocummentFactory::creatDocumment(DocType_EM type, DWidget *father)
{
    DocummentBase *documment;
    switch (type) {
    case DocType_PDF:
        documment = new DocummentPDF(father);
        break;
    case DocType_DJVU:
        documment = new DocummentDJVU(father);
        break;
    default:
        break;
    }
    return documment;
}
