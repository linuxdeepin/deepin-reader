// Copyright 2014 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/jbig2/JBig2_SymbolDict.h"

#include "core/fxcodec/jbig2/JBig2_Image.h"

CJBig2_SymbolDict::CJBig2_SymbolDict() {}

CJBig2_SymbolDict::~CJBig2_SymbolDict() = default;

std::unique_ptr<CJBig2_SymbolDict> CJBig2_SymbolDict::DeepCopy() const {
  auto dst = std::make_unique<CJBig2_SymbolDict>();
  for (const auto& image : m_SDEXSYMS) {
    dst->m_SDEXSYMS.push_back(image ? std::make_unique<CJBig2_Image>(*image)
                                    : nullptr);
  }
  dst->m_gbContext = m_gbContext;
  dst->m_grContext = m_grContext;
  return dst;
}
