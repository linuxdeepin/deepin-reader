/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ut_json.h"
#include "app/Json.h"

#include <QStringList>
#include <QJsonArray>
#include <QDebug>

Ut_Json::Ut_Json()
{
}

void Ut_Json::SetUp()
{
}

void Ut_Json::TearDown()
{
}

#ifdef UT_JSON_TEST
TEST_F(Ut_Json, JsonTest)
{
    Json json("{\"test0_key\" : \"test0_value\", \"test1_key\" : 1, \"test2_key\" : true, \"test3_key\" : 520.1314, \"test4_key\" : [\"1\", \"2\",\"3\"], \"test5_key\" : {\"child0\" : 1}}");
    EXPECT_STREQ(json.getString("test0_key").toStdString().c_str(), "test0_value");
    EXPECT_EQ(json.getInt("test1_key"), 1);
    EXPECT_EQ(json.getBool("test2_key"), true);
    EXPECT_DOUBLE_EQ(json.getDouble("test3_key"), 520.1314);
    EXPECT_EQ(json.getStringList("test4_key").join(","), "1,2,3");
    EXPECT_EQ(json.getJsonArray("test4_key"), QJsonArray() << "1"
                                                           << "2"
                                                           << "3");
    EXPECT_EQ(json.getJsonValue("test1_key").toInt(), 1);
    EXPECT_EQ(json.getJsonObject("test5_key").value("child0"), 1);

    EXPECT_EQ(json.getInt("test5_key.child0"), 1);

    json.set("test0_key", "111");
    EXPECT_STREQ(json.getString("test0_key").toStdString().c_str(), "111");
    json.set("test0_key", QStringList() << "1"
                                        << "2"
                                        << "3");
    EXPECT_STREQ(json.getStringList("test0_key").join(",").toStdString().c_str(), "1,2,3");
    json.set("test0_key", "test0_value");
    EXPECT_STREQ(json.toString(QJsonDocument::Compact).toStdString().c_str(), "{\"test0_key\":\"test0_value\",\"test1_key\":1,\"test2_key\":true,\"test3_key\":520.1314,\"test4_key\":[\"1\",\"2\",\"3\"],\"test5_key\":{\"child0\":1}}");
}
#endif
