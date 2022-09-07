// Copyright 2014 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/formfiller/cffl_pushbutton.h"

#include <utility>

#include "fpdfsdk/formfiller/cffl_formfiller.h"
#include "fpdfsdk/pwl/cpwl_special_button.h"

CFFL_PushButton::CFFL_PushButton(CPDFSDK_FormFillEnvironment* pApp,
                                 CPDFSDK_Widget* pWidget)
    : CFFL_Button(pApp, pWidget) {}

CFFL_PushButton::~CFFL_PushButton() = default;

std::unique_ptr<CPWL_Wnd> CFFL_PushButton::NewPWLWindow(
    const CPWL_Wnd::CreateParams& cp,
    std::unique_ptr<IPWL_SystemHandler::PerWindowData> pAttachedData) {
  auto pWnd = std::make_unique<CPWL_PushButton>(cp, std::move(pAttachedData));
  pWnd->Realize();
  return std::move(pWnd);
}
