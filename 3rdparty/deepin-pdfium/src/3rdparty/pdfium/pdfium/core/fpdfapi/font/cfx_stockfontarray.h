// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FONT_CFX_STOCKFONTARRAY_H_
#define CORE_FPDFAPI_FONT_CFX_STOCKFONTARRAY_H_

#include "core/fxcrt/retain_ptr.h"
#include "core/fxge/cfx_fontmapper.h"

class CPDF_Font;

class CFX_StockFontArray {
 public:
  CFX_StockFontArray();
  ~CFX_StockFontArray();

  RetainPtr<CPDF_Font> GetFont(CFX_FontMapper::StandardFont index) const;
  void SetFont(CFX_FontMapper::StandardFont index,
               const RetainPtr<CPDF_Font>& pFont);

 private:
  RetainPtr<CPDF_Font> m_StockFonts[14];
};

#endif  // CORE_FPDFAPI_FONT_CFX_STOCKFONTARRAY_H_
