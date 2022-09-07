// Copyright 2018 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_annotcontext.h"

#include "core/fpdfapi/page/cpdf_form.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_stream.h"

CPDF_AnnotContext::CPDF_AnnotContext(CPDF_Dictionary* pAnnotDict,
                                     IPDF_Page* pPage)
    : m_pAnnotDict(pAnnotDict), m_pPage(pPage) {
  ASSERT(m_pAnnotDict);
  ASSERT(m_pPage);
  ASSERT(m_pPage->AsPDFPage());
}

CPDF_AnnotContext::~CPDF_AnnotContext() = default;

void CPDF_AnnotContext::SetForm(CPDF_Stream* pStream) {
  if (!pStream)
    return;

  // Reset the annotation matrix to be the identity matrix, since the
  // appearance stream already takes matrix into account.
  pStream->GetDict()->SetMatrixFor("Matrix", CFX_Matrix());

  m_pAnnotForm = std::make_unique<CPDF_Form>(
      m_pPage->GetDocument(), m_pPage->AsPDFPage()->m_pResources.Get(),
      pStream);
  m_pAnnotForm->ParseContent();
}
