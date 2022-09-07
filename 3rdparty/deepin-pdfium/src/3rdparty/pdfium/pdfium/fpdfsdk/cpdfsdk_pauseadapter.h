// Copyright 2016 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_CPDFSDK_PAUSEADAPTER_H_
#define FPDFSDK_CPDFSDK_PAUSEADAPTER_H_

#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/pauseindicator_iface.h"
#include "core/fxcrt/unowned_ptr.h"
#include "public/fpdf_progressive.h"

class CPDFSDK_PauseAdapter final : public PauseIndicatorIface {
 public:
  explicit CPDFSDK_PauseAdapter(IFSDK_PAUSE* IPause);
  ~CPDFSDK_PauseAdapter() override;

  bool NeedToPauseNow() override;

 private:
  UnownedPtr<IFSDK_PAUSE> const m_IPause;
};

#endif  // FPDFSDK_CPDFSDK_PAUSEADAPTER_H_
