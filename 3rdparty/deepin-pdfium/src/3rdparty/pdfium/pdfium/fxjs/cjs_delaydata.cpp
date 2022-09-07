// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_delaydata.h"

CJS_DelayData::CJS_DelayData(FIELD_PROP prop, int idx, const WideString& name)
    : eProp(prop), nControlIndex(idx), sFieldName(name) {}

CJS_DelayData::~CJS_DelayData() = default;
