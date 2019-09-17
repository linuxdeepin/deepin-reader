#include "ThumbnailLoadThread.h"

ThumbnailLoadThread::ThumbnailLoadThread()
{

}

void ThumbnailLoadThread::run()
{
    bool bl = true;
    while (bl) {
        //to do
        if (m_pThumbnailWidget) {
            bl = m_pThumbnailWidget->fillContantToList();
        } else {
            bl = false;
        }
    }
}
