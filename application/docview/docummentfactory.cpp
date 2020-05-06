#include "docummentfactory.h"
#include "pdf/docummentpdf.h"
#include "djvu/docummentdjvu.h"
#include "tiff/documentiff.h"
#include "ps/docummentps.h"
#include "xps/docummentxps.h"


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
