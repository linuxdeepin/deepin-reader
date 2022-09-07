// Copyright 2016 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_SIMPLE_PARSER_H_
#define CORE_FPDFAPI_PARSER_CPDF_SIMPLE_PARSER_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "third_party/base/span.h"

class CPDF_SimpleParser {
 public:
  explicit CPDF_SimpleParser(pdfium::span<const uint8_t> input);
  ~CPDF_SimpleParser();

  ByteStringView GetWord();

  void SetCurPos(uint32_t pos) { cur_pos_ = pos; }
  uint32_t GetCurPos() const { return cur_pos_; }

 private:
  const pdfium::span<const uint8_t> data_;
  uint32_t cur_pos_ = 0;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_SIMPLE_PARSER_H_
