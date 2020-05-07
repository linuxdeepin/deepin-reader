#ifndef SHEETBROWSERDJVU_H
#define SHEETBROWSERDJVU_H

#include <QGraphicsView>
#include "global.h"

namespace deepin_reader {
class DjVuDocument;
}

class SheetBrowserDJVUItem;
class SheetBrowserDJVU : public QGraphicsView
{
    Q_OBJECT
public:
    explicit SheetBrowserDJVU(QWidget *parent = nullptr);

    ~SheetBrowserDJVU();

    bool openFilePath(const QString &);

    void loadPages();

    void setScale(Dr::ScaleMode mode, double scale, Dr::Rotation rotation);

private:
    deepin_reader::DjVuDocument *m_document = nullptr;
    QList<SheetBrowserDJVUItem *> m_items;
    Dr::Rotation m_rotion = Dr::RotateBy0;
    Dr::ScaleMode m_mode = Dr::ScaleFactorMode;
    double m_scale = 100.0;

    int m_maxWidth = 0;
    int m_maxHeight = 0;
};

#endif // SHEETBROWSERDJVU_H
