// Copyright 2018 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/fpdfapi/edit/cpdf_stringarchivestream.h"

CPDF_StringArchiveStream::CPDF_StringArchiveStream(std::ostringstream* stream)
    : stream_(stream) {}

CPDF_StringArchiveStream::~CPDF_StringArchiveStream() = default;

bool CPDF_StringArchiveStream::WriteByte(uint8_t byte) {
  NOTREACHED();
  return false;
}

bool CPDF_StringArchiveStream::WriteDWord(uint32_t i) {
  NOTREACHED();
  return false;
}

FX_FILESIZE CPDF_StringArchiveStream::CurrentOffset() const {
  NOTREACHED();
  return false;
}

bool CPDF_StringArchiveStream::WriteBlock(const void* pData, size_t size) {
  stream_->write(static_cast<const char*>(pData), size);
  return true;
}

bool CPDF_StringArchiveStream::WriteString(ByteStringView str) {
  stream_->write(str.unterminated_c_str(), str.GetLength());
  return true;
}
