// Copyright 2015 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FPDFSDK_FPDF_VIEW_C_API_TEST_H_
#define FPDFSDK_FPDF_VIEW_C_API_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

// Function to call from gtest harness to ensure linker resolution. Returns
// 1 on success or 0 on error.
int CheckPDFiumCApi();

#ifdef __cplusplus
}
#endif

#endif  // FPDFSDK_FPDF_VIEW_C_API_TEST_H_
