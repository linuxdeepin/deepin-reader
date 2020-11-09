#ifndef DPDFGLOBAL_H
#define DPDFGLOBAL_H

#include <QtCore/qglobal.h>

#ifndef BUILD_DEEPDF_STATIC
#    if defined(BUILD_DEEPDF_LIB)
#      define DEEPDF_EXPORT Q_DECL_EXPORT
#    else
#      define DEEPDF_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define DEEPDF_EXPORT
#endif

class DPDFGlobal
{
public:
    DPDFGlobal();

    ~DPDFGlobal();

private:
    void init();

    void destory();
};

#endif // DPDFGLOBAL_H
