#ifndef DOCSHEETDJVU_H
#define DOCSHEETDJVU_H

#include "DocSheet.h"

class DocSheetDJVU : public DocSheet
{
    Q_OBJECT
public:
    explicit DocSheetDJVU(QWidget *parent = nullptr);

    bool openFileExec(const QString &filePath);

};

#endif // DOCSHEETDJVU_H
