/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     gaoxiang <gaoxiang@uniontech.com>
*
* Maintainer: gaoxiang <gaoxiang@uniontech.com>
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

#include "SideBarImageViewModel.h"
#include "DocSheet.h"
#include "SideBarImageViewModel.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>
#include <QListView>

class TestImagePageInfo_t : public ::testing::Test
{
public:
    TestImagePageInfo_t(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new ImagePageInfo_t();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    ImagePageInfo_t *m_tester;
};

TEST_F(TestImagePageInfo_t, initTest)
{

}

TEST_F(TestImagePageInfo_t, test1)
{
    ImagePageInfo_t temp;
    m_tester == &temp;
}

TEST_F(TestImagePageInfo_t, test2)
{
    ImagePageInfo_t temp;
    m_tester < &temp;
}

TEST_F(TestImagePageInfo_t, test3)
{
    ImagePageInfo_t temp;
    m_tester > &temp;
}



class TestSideBarImageViewModel : public ::testing::Test
{
public:
    TestSideBarImageViewModel(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new SideBarImageViewModel(sheet);
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    SideBarImageViewModel *m_tester;
};

TEST_F(TestSideBarImageViewModel, inittest)
{

}

TEST_F(TestSideBarImageViewModel, testresetData)
{
    m_tester->resetData();
}

TEST_F(TestSideBarImageViewModel, testinitModelLst)
{
    m_tester->initModelLst(QList<ImagePageInfo_t>(), true);
}

TEST_F(TestSideBarImageViewModel, testchangeModelData)
{
    m_tester->changeModelData(QList<ImagePageInfo_t>());
}

TEST_F(TestSideBarImageViewModel, testsetBookMarkVisible)
{
    m_tester->setBookMarkVisible(0, true, true);
}

TEST_F(TestSideBarImageViewModel, testrowCount)
{
    m_tester->rowCount(QModelIndex());
}

TEST_F(TestSideBarImageViewModel, testcolumnCount)
{
    m_tester->columnCount(QModelIndex());
}

TEST_F(TestSideBarImageViewModel, testdata)
{
    m_tester->data(QModelIndex(), Qt::DisplayRole);
}

TEST_F(TestSideBarImageViewModel, testsetData)
{
    m_tester->setData(QModelIndex(), "", Qt::DisplayRole);
}

TEST_F(TestSideBarImageViewModel, testgetModelIndexForPageIndex)
{
    m_tester->getModelIndexForPageIndex(0);
}

TEST_F(TestSideBarImageViewModel, testgetPageIndexForModelIndex)
{
    m_tester->getPageIndexForModelIndex(0);
}

TEST_F(TestSideBarImageViewModel, testonUpdateImage)
{
    m_tester->onUpdateImage(0);
}

TEST_F(TestSideBarImageViewModel, testinsertPageIndex)
{
    m_tester->insertPageIndex(0);
}

TEST_F(TestSideBarImageViewModel, testinsertPageIndex1)
{
    m_tester->insertPageIndex(ImagePageInfo_t());
}

TEST_F(TestSideBarImageViewModel, testremovePageIndex)
{
    m_tester->removePageIndex(0);
}

TEST_F(TestSideBarImageViewModel, testremoveItemForAnno)
{
    m_tester->removeItemForAnno(nullptr);
}

TEST_F(TestSideBarImageViewModel, testgetModelIndexImageInfo)
{
    ImagePageInfo_t temp;
    m_tester->getModelIndexImageInfo(0, temp);
}

TEST_F(TestSideBarImageViewModel, testfindItemForAnno)
{
    m_tester->findItemForAnno(nullptr);
}

TEST_F(TestSideBarImageViewModel, testhandleRenderThumbnail)
{
    m_tester->handleRenderThumbnail(0, QPixmap());
}
