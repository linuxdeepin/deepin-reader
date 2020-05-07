#ifndef SHEETBROWSERDJVU_H
#define SHEETBROWSERDJVU_H

#include <QGraphicsView>

namespace deepin_reader {
class DjVuDocument;
}

class SheetBrowserDJVU : public QGraphicsView
{
    Q_OBJECT
public:
    explicit SheetBrowserDJVU(QWidget *parent = nullptr);

    bool openFilePath(const QString &);

    void loadPages();

private:
    deepin_reader::DjVuDocument *m_document = nullptr;
};

#endif // SHEETBROWSERDJVU_H
