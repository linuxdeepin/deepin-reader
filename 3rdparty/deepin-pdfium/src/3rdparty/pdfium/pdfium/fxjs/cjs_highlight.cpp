// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_highlight.h"

const JSConstSpec CJS_Highlight::ConstSpecs[] = {
    {"n", JSConstSpec::String, 0, "none"},
    {"i", JSConstSpec::String, 0, "invert"},
    {"p", JSConstSpec::String, 0, "push"},
    {"o", JSConstSpec::String, 0, "outline"}};

int CJS_Highlight::ObjDefnID = -1;

// static
void CJS_Highlight::DefineJSObjects(CFXJS_Engine* pEngine) {
  ObjDefnID =
      pEngine->DefineObj("highlight", FXJSOBJTYPE_STATIC, nullptr, nullptr);
  DefineConsts(pEngine, ObjDefnID, ConstSpecs);
}
