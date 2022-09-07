// Copyright 2019 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_TEXT_GLYPH_POS_H_
#define CORE_FXGE_TEXT_GLYPH_POS_H_

#include "core/fxcrt/fx_coordinates.h"

#include "core/fxcrt/unowned_ptr.h"
#include "third_party/base/optional.h"

class CFX_GlyphBitmap;

class TextGlyphPos {
 public:
  TextGlyphPos();
  TextGlyphPos(const TextGlyphPos&);
  ~TextGlyphPos();

  Optional<CFX_Point> GetOrigin(const CFX_Point& offset) const;

  UnownedPtr<const CFX_GlyphBitmap> m_pGlyph;
  CFX_Point m_Origin;
  CFX_PointF m_fDeviceOrigin;
};

#endif  // CORE_FXGE_TEXT_GLYPH_POS_H_
