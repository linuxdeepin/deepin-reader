// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CJS_GLOBALCONSTS_H_
#define FXJS_CJS_GLOBALCONSTS_H_

#include "fxjs/cjs_object.h"

class CJS_GlobalConsts final : public CJS_Object {
 public:
  static void DefineJSObjects(CJS_Runtime* pRuntime);
};

#endif  // FXJS_CJS_GLOBALCONSTS_H_
