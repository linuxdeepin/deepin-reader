// Copyright 2018 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CORE_FPDFAPI_EDIT_CPDF_STRINGARCHIVESTREAM_H_
#define CORE_FPDFAPI_EDIT_CPDF_STRINGARCHIVESTREAM_H_

#include "core/fxcrt/fx_stream.h"

class CPDF_StringArchiveStream final : public IFX_ArchiveStream {
 public:
  explicit CPDF_StringArchiveStream(std::ostringstream* stream);
  ~CPDF_StringArchiveStream() override;

  // IFX_ArchiveStream
  bool WriteByte(uint8_t byte) override;
  bool WriteDWord(uint32_t i) override;
  FX_FILESIZE CurrentOffset() const override;
  bool WriteBlock(const void* pData, size_t size) override;
  bool WriteString(ByteStringView str) override;

 private:
  std::ostringstream* stream_;
};

#endif  // CORE_FPDFAPI_EDIT_CPDF_STRINGARCHIVESTREAM_H_
