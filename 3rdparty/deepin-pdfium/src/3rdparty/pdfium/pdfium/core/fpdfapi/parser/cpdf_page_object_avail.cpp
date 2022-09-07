// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/fpdfapi/parser/cpdf_page_object_avail.h"

#include "core/fpdfapi/parser/cpdf_dictionary.h"

CPDF_PageObjectAvail::~CPDF_PageObjectAvail() = default;

bool CPDF_PageObjectAvail::ExcludeObject(const CPDF_Object* object) const {
  if (CPDF_ObjectAvail::ExcludeObject(object))
    return true;

  return object->IsDictionary() &&
         object->GetDict()->GetNameFor("Type") == "Page";
}
