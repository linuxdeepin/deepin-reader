// Copyright 2016 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_null.h"

#include "core/fxcrt/fx_stream.h"

CPDF_Null::CPDF_Null() {}

CPDF_Object::Type CPDF_Null::GetType() const {
  return kNullobj;
}

RetainPtr<CPDF_Object> CPDF_Null::Clone() const {
  return pdfium::MakeRetain<CPDF_Null>();
}

bool CPDF_Null::WriteTo(IFX_ArchiveStream* archive,
                        const CPDF_Encryptor* encryptor) const {
  return archive->WriteString(" null");
}

bool CPDF_Null::IsNull() const {
  return true;
}
