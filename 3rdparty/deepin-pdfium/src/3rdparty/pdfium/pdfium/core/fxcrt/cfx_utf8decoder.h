// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CFX_UTF8DECODER_H_
#define CORE_FXCRT_CFX_UTF8DECODER_H_

#include "core/fxcrt/cfx_widetextbuf.h"

class CFX_UTF8Decoder {
 public:
  CFX_UTF8Decoder();
  ~CFX_UTF8Decoder();

  void Input(uint8_t byte);
  void AppendCodePoint(uint32_t ch);
  void ClearStatus() { m_PendingBytes = 0; }
  WideStringView GetResult() const { return m_Buffer.AsStringView(); }

 private:
  int m_PendingBytes = 0;
  uint32_t m_PendingChar = 0;
  CFX_WideTextBuf m_Buffer;
};

#endif  // CORE_FXCRT_CFX_UTF8DECODER_H_
