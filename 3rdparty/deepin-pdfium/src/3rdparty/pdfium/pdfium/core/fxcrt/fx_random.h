// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_FX_RANDOM_H_
#define CORE_FXCRT_FX_RANDOM_H_

#include <stdint.h>

void* FX_Random_MT_Start(uint32_t dwSeed);
void FX_Random_MT_Close(void* pContext);
uint32_t FX_Random_MT_Generate(void* pContext);

void FX_Random_GenerateMT(uint32_t* pBuffer, int32_t iCount);

#endif  // CORE_FXCRT_FX_RANDOM_H_
