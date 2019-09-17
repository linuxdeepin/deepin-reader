#ifndef THUMBNAILLOADTHREAD_H
#define THUMBNAILLOADTHREAD_H

#include <QThread>

#include "ThumbnailWidget.h"

class ThumbnailLoadThread :  public QThread
{
public:
    ThumbnailLoadThread();

public:
    inline void setThumbnailP(ThumbnailWidget *thumbnail)
    {
        m_pThumbnailWidget = thumbnail;
    }

protected:
    virtual void run() override;

private:
    ThumbnailWidget *m_pThumbnailWidget = nullptr;
};

#endif // THUMBNAILLOADTHREAD_H
