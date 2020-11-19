#ifndef UT_APP_H
#define UT_APP_H

#include "ut_defines.h"

#include <gtest/gtest.h>

class ut_app : public ::testing::Test
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
