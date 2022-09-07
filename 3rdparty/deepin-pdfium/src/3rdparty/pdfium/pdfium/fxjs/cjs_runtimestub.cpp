// Copyright 2015 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_runtimestub.h"

#include "fxjs/cjs_event_context_stub.h"

CJS_RuntimeStub::CJS_RuntimeStub(CPDFSDK_FormFillEnvironment* pFormFillEnv)
    : m_pFormFillEnv(pFormFillEnv) {}

CJS_RuntimeStub::~CJS_RuntimeStub() = default;

IJS_EventContext* CJS_RuntimeStub::NewEventContext() {
  if (!m_pContext)
    m_pContext = std::make_unique<CJS_EventContextStub>();
  return m_pContext.get();
}

void CJS_RuntimeStub::ReleaseEventContext(IJS_EventContext* pContext) {}

CPDFSDK_FormFillEnvironment* CJS_RuntimeStub::GetFormFillEnv() const {
  return m_pFormFillEnv.Get();
}

CJS_Runtime* CJS_RuntimeStub::AsCJSRuntime() {
  return nullptr;
}

Optional<IJS_Runtime::JS_Error> CJS_RuntimeStub::ExecuteScript(
    const WideString& script) {
  return pdfium::nullopt;
}
