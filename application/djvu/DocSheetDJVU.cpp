#include "DocSheetDJVU.h"

DocSheetDJVU::DocSheetDJVU(QWidget *parent) : DocSheet(Dr::DjVu, parent)
{

}

bool DocSheetDJVU::openFileExec(const QString &filePath)
{
    return true;
}
