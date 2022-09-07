// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_PAUSEINDICATOR_IFACE_H_
#define CORE_FXCRT_PAUSEINDICATOR_IFACE_H_

class PauseIndicatorIface {
 public:
  virtual ~PauseIndicatorIface() = default;
  virtual bool NeedToPauseNow() = 0;
};

#endif  // CORE_FXCRT_PAUSEINDICATOR_IFACE_H_
