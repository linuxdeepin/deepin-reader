#ifndef UT_BROWSER_H
#define UT_BROWSER_H

#include "ut_defines.h"
#include "ut_application.h"

class ut_browser : public ut_application
{
public:
    ut_browser();

public:
    //用于做一些初始化操作
    virtual void SetUp();

    //用于做一些清理操作
    virtual void TearDown();
};

#endif // UT_BROWSER_H
