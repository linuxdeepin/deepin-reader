// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_scalehow.h"

const JSConstSpec CJS_ScaleHow::ConstSpecs[] = {
    {"proportional", JSConstSpec::Number, 0, 0},
    {"anamorphic", JSConstSpec::Number, 1, 0}};

int CJS_ScaleHow::ObjDefnID = -1;

// static
void CJS_ScaleHow::DefineJSObjects(CFXJS_Engine* pEngine) {
  ObjDefnID =
      pEngine->DefineObj("scaleHow", FXJSOBJTYPE_STATIC, nullptr, nullptr);
  DefineConsts(pEngine, ObjDefnID, ConstSpecs);
}
