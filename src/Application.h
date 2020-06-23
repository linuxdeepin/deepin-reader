#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <DApplication>
#include "pdfControl/database/DBService.h"

class AppInfo;

#if defined(dApp)
#undef dApp
#endif
#define dApp (static_cast<Application *>(QCoreApplication::instance()))

DWIDGET_USE_NAMESPACE

class Application : public DApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);

    void setSreenRect(const QRect &rect);

protected:
    void handleQuitAction() override;

public:
    DBService *m_pDBService = nullptr;

    AppInfo   *m_pAppInfo = nullptr;

private:
    void initCfgPath();

    void initChildren();
};

#endif  // APPLICATION_H_
