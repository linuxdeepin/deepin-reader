// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_zoomtype.h"

const JSConstSpec CJS_Zoomtype::ConstSpecs[] = {
    {"none", JSConstSpec::String, 0, "NoVary"},
    {"fitP", JSConstSpec::String, 0, "FitPage"},
    {"fitW", JSConstSpec::String, 0, "FitWidth"},
    {"fitH", JSConstSpec::String, 0, "FitHeight"},
    {"fitV", JSConstSpec::String, 0, "FitVisibleWidth"},
    {"pref", JSConstSpec::String, 0, "Preferred"},
    {"refW", JSConstSpec::String, 0, "ReflowWidth"}};

int CJS_Zoomtype::ObjDefnID = -1;

// static
void CJS_Zoomtype::DefineJSObjects(CFXJS_Engine* pEngine) {
  ObjDefnID =
      pEngine->DefineObj("zoomtype", FXJSOBJTYPE_STATIC, nullptr, nullptr);
  DefineConsts(pEngine, ObjDefnID, ConstSpecs);
}
