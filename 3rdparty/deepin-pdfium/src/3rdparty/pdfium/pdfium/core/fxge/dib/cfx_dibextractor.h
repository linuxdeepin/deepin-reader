// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_DIB_CFX_DIBEXTRACTOR_H_
#define CORE_FXGE_DIB_CFX_DIBEXTRACTOR_H_

#include "core/fxcrt/retain_ptr.h"

class CFX_DIBBase;
class CFX_DIBitmap;

class CFX_DIBExtractor {
 public:
  explicit CFX_DIBExtractor(const RetainPtr<CFX_DIBBase>& pSrc);
  ~CFX_DIBExtractor();

  RetainPtr<CFX_DIBitmap> GetBitmap() { return m_pBitmap; }

 private:
  RetainPtr<CFX_DIBitmap> m_pBitmap;
};

#endif  // CORE_FXGE_DIB_CFX_DIBEXTRACTOR_H_
