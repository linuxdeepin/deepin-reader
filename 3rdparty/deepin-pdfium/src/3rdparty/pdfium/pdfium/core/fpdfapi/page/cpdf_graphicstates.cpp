// Copyright 2016 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_graphicstates.h"

CPDF_GraphicStates::CPDF_GraphicStates() {}

CPDF_GraphicStates::~CPDF_GraphicStates() = default;

void CPDF_GraphicStates::DefaultStates() {
  m_ColorState.Emplace();
  m_ColorState.SetDefault();
}

void CPDF_GraphicStates::CopyStates(const CPDF_GraphicStates& src) {
  m_ClipPath = src.m_ClipPath;
  m_GraphState = src.m_GraphState;
  m_ColorState = src.m_ColorState;
  m_TextState = src.m_TextState;
  m_GeneralState = src.m_GeneralState;
}
