/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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

#include "IHelper.h"

#include "HelperImpl.h"
#include "AnnotationHelper.h"
#include "DocFileHelper.h"

IHelper::IHelper()
{

}

Helper::Helper()
{
    m_pAnnotatinHelperImpl = new AnnotationHelper;
    m_pDocHelperImpl = new DocFileHelper;
}

void Helper::qDealWithData(const int &msgType, const QString &msgContent)
{
    int nRes = m_pAnnotatinHelperImpl->qDealWithData(msgType, msgContent);
    if (nRes == MSG_NO_OK) {
        m_pDocHelperImpl->qDealWithData(msgType, msgContent);
    }
}
