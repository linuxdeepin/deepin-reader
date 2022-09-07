// Copyright 2016 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPVT_GENERATEAP_H_
#define CORE_FPDFDOC_CPVT_GENERATEAP_H_

#include "core/fpdfdoc/cpdf_annot.h"
#include "core/fxcrt/fx_system.h"

class CPDF_Dictionary;
class CPDF_Document;

class CPVT_GenerateAP {
 public:
  enum FormType { kTextField, kComboBox, kListBox };

  static void GenerateFormAP(CPDF_Document* pDoc,
                             CPDF_Dictionary* pAnnotDict,
                             FormType type);

  static void GenerateEmptyAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict);

  static bool GenerateAnnotAP(CPDF_Document* pDoc,
                              CPDF_Dictionary* pAnnotDict,
                              CPDF_Annot::Subtype subtype);

  CPVT_GenerateAP() = delete;
  CPVT_GenerateAP(const CPVT_GenerateAP&) = delete;
  CPVT_GenerateAP& operator=(const CPVT_GenerateAP&) = delete;
};

#endif  // CORE_FPDFDOC_CPVT_GENERATEAP_H_
