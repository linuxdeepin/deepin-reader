// Copyright 2016 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_BASIC_BASICMODULE_H_
#define CORE_FXCODEC_BASIC_BASICMODULE_H_

#include <memory>

#include "core/fxcrt/fx_memory_wrappers.h"
#include "core/fxcrt/fx_system.h"
#include "third_party/base/span.h"

namespace fxcodec {

class ScanlineDecoder;

class BasicModule {
 public:
  static std::unique_ptr<ScanlineDecoder> CreateRunLengthDecoder(
      pdfium::span<const uint8_t> src_buf,
      int width,
      int height,
      int nComps,
      int bpc);

  static bool RunLengthEncode(pdfium::span<const uint8_t> src_span,
                              std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                              uint32_t* dest_size);

  static bool A85Encode(pdfium::span<const uint8_t> src_span,
                        std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                        uint32_t* dest_size);

  BasicModule() = delete;
  BasicModule(const BasicModule&) = delete;
  BasicModule& operator=(const BasicModule&) = delete;
};

}  // namespace fxcodec

using BasicModule = fxcodec::BasicModule;

#endif  // CORE_FXCODEC_BASIC_BASICMODULE_H_
