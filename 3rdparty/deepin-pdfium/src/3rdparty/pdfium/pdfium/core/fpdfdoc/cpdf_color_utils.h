// Copyright 2019 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_COLOR_UTILS_H_
#define CORE_FPDFDOC_CPDF_COLOR_UTILS_H_

#include "core/fxge/cfx_color.h"

class CPDF_Array;

namespace fxcrt {
class ByteString;
}

namespace fpdfdoc {

CFX_Color CFXColorFromArray(const CPDF_Array& array);
CFX_Color CFXColorFromString(const fxcrt::ByteString& str);

}  // namespace fpdfdoc

#endif  // CORE_FPDFDOC_CPDF_COLOR_UTILS_H_
