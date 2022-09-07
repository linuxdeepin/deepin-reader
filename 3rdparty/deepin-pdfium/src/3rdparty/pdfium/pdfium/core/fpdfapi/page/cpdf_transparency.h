// Copyright 2018 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CORE_FPDFAPI_PAGE_CPDF_TRANSPARENCY_H_
#define CORE_FPDFAPI_PAGE_CPDF_TRANSPARENCY_H_

class CPDF_Transparency {
 public:
  CPDF_Transparency();

  CPDF_Transparency(const CPDF_Transparency& other);

  bool IsGroup() const { return m_bGroup; }
  bool IsIsolated() const { return m_bIsolated; }

  void SetGroup() { m_bGroup = true; }
  void SetIsolated() { m_bIsolated = true; }

 private:
  bool m_bGroup = false;
  bool m_bIsolated = false;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_TRANSPARENCY_H_
