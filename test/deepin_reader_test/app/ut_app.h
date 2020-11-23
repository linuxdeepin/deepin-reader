#ifndef UT_APP_H
#define UT_APP_H

#include "ut_defines.h"
#include "ut_application.h"

#include <gtest/gtest.h>

class ut_app : public ut_application
{
public:
    ut_app();

public:
    //用于做一些初始化操作
    virtual void SetUp();

    //用于做一些清理操作
    virtual void TearDown();
};

#endif // UT_APP_H
