// Copyright 2018 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_ANNOTCONTEXT_H_
#define CORE_FPDFAPI_PAGE_CPDF_ANNOTCONTEXT_H_

#include <memory>

#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_Dictionary;
class CPDF_Form;
class CPDF_Stream;
class IPDF_Page;

class CPDF_AnnotContext {
 public:
  CPDF_AnnotContext(CPDF_Dictionary* pAnnotDict, IPDF_Page* pPage);
  ~CPDF_AnnotContext();

  void SetForm(CPDF_Stream* pStream);
  bool HasForm() const { return !!m_pAnnotForm; }
  CPDF_Form* GetForm() const { return m_pAnnotForm.get(); }

  // Never nullptr.
  CPDF_Dictionary* GetAnnotDict() const { return m_pAnnotDict.Get(); }

  // Never nullptr.
  IPDF_Page* GetPage() const { return m_pPage.Get(); }

 private:
  std::unique_ptr<CPDF_Form> m_pAnnotForm;
  RetainPtr<CPDF_Dictionary> const m_pAnnotDict;
  UnownedPtr<IPDF_Page> const m_pPage;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_ANNOTCONTEXT_H_
