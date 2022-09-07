// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_style.h"

const JSConstSpec CJS_Style::ConstSpecs[] = {
    {"ch", JSConstSpec::String, 0, "check"},
    {"cr", JSConstSpec::String, 0, "cross"},
    {"di", JSConstSpec::String, 0, "diamond"},
    {"ci", JSConstSpec::String, 0, "circle"},
    {"st", JSConstSpec::String, 0, "star"},
    {"sq", JSConstSpec::String, 0, "square"}};

int CJS_Style::ObjDefnID = -1;

// static
void CJS_Style::DefineJSObjects(CFXJS_Engine* pEngine) {
  ObjDefnID = pEngine->DefineObj("style", FXJSOBJTYPE_STATIC, nullptr, nullptr);
  DefineConsts(pEngine, ObjDefnID, ConstSpecs);
}
