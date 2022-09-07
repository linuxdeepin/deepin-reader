// Copyright 2019 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_TIMERHANDLER_IFACE_H_
#define CORE_FXCRT_TIMERHANDLER_IFACE_H_

#include "core/fxcrt/fx_system.h"

namespace fxcrt {

class TimerHandlerIface {
 public:
  static constexpr int32_t kInvalidTimerID = 0;
  using TimerCallback = void (*)(int32_t idEvent);

  virtual ~TimerHandlerIface() = default;

  virtual int32_t SetTimer(int32_t uElapse, TimerCallback lpTimerFunc) = 0;
  virtual void KillTimer(int32_t nTimerID) = 0;
};

}  // namespace fxcrt

using fxcrt::TimerHandlerIface;

#endif  // CORE_FXCRT_TIMERHANDLER_IFACE_H_
