// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_psfunc.h"

#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"

CPDF_PSFunc::CPDF_PSFunc() : CPDF_Function(Type::kType4PostScript) {}

CPDF_PSFunc::~CPDF_PSFunc() = default;

bool CPDF_PSFunc::v_Init(const CPDF_Object* pObj,
                         std::set<const CPDF_Object*>* pVisited) {
  auto pAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pObj->AsStream());
  pAcc->LoadAllDataFiltered();
  return m_PS.Parse(pAcc->GetSpan());
}

bool CPDF_PSFunc::v_Call(const float* inputs, float* results) const {
  m_PS.Reset();
  for (uint32_t i = 0; i < m_nInputs; i++)
    m_PS.Push(inputs[i]);
  m_PS.Execute();
  if (m_PS.GetStackSize() < m_nOutputs)
    return false;
  for (uint32_t i = 0; i < m_nOutputs; i++)
    results[m_nOutputs - i - 1] = m_PS.Pop();
  return true;
}
