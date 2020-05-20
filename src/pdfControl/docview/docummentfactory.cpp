#include "docummentfactory.h"
#include "pdf/docummentpdf.h"

DocummentFactory::DocummentFactory()
{

}

DocummentBase *DocummentFactory::creatDocumment(Dr::FileType type, DWidget *father)
{
    DocummentBase *documment = nullptr;
    switch (type) {
    case Dr::PDF:
        documment = new DocummentPDF(father);
        break;
    default:
        break;
    }
    return documment;
}
