// Copyright 2016 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_GRAPHICSTATES_H_
#define CORE_FPDFAPI_PAGE_CPDF_GRAPHICSTATES_H_

#include "core/fpdfapi/page/cpdf_clippath.h"
#include "core/fpdfapi/page/cpdf_colorstate.h"
#include "core/fpdfapi/page/cpdf_generalstate.h"
#include "core/fpdfapi/page/cpdf_textstate.h"
#include "core/fxge/cfx_graphstate.h"

class CPDF_GraphicStates {
 public:
  CPDF_GraphicStates();
  virtual ~CPDF_GraphicStates();

  void CopyStates(const CPDF_GraphicStates& src);
  void DefaultStates();

  CPDF_ClipPath m_ClipPath;
  CFX_GraphState m_GraphState;
  CPDF_ColorState m_ColorState;
  CPDF_TextState m_TextState;
  CPDF_GeneralState m_GeneralState;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_GRAPHICSTATES_H_
