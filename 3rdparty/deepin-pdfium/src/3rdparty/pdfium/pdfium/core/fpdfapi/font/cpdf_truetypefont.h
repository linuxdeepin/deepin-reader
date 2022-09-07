// Copyright 2016 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FONT_CPDF_TRUETYPEFONT_H_
#define CORE_FPDFAPI_FONT_CPDF_TRUETYPEFONT_H_

#include "core/fpdfapi/font/cpdf_simplefont.h"
#include "core/fxcrt/fx_system.h"

class CPDF_TrueTypeFont final : public CPDF_SimpleFont {
 public:
  CONSTRUCT_VIA_MAKE_RETAIN;
  ~CPDF_TrueTypeFont() override;

  // CPDF_Font:
  bool IsTrueTypeFont() const override;
  const CPDF_TrueTypeFont* AsTrueTypeFont() const override;
  CPDF_TrueTypeFont* AsTrueTypeFont() override;

 private:
  CPDF_TrueTypeFont(CPDF_Document* pDocument, CPDF_Dictionary* pFontDict);

  // CPDF_Font:
  bool Load() override;

  // CPDF_SimpleFont:
  void LoadGlyphMap() override;
};

#endif  // CORE_FPDFAPI_FONT_CPDF_TRUETYPEFONT_H_
