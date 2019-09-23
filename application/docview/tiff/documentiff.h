#ifndef DOCUMENTIFF_H
#define DOCUMENTIFF_H

#include "../docummentbase.h"
#include <AbstractAppender.h>
#include <tiff.h>
#include <tiffio.h>


class DocumenTiff: public DocummentBase
{
    friend class PageTiff;
public:
    DocumenTiff(DWidget *parent = nullptr);
    bool openFile(QString filepath) override;
//    bool loadPages() override;

    TIFF *document;
};

#endif // DOCUMENTIFF_H
