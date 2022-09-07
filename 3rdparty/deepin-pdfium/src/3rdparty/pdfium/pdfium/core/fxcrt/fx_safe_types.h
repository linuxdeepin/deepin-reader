// Copyright 2014 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CORE_FXCRT_FX_SAFE_TYPES_H_
#define CORE_FXCRT_FX_SAFE_TYPES_H_

#include <stdlib.h>  // For size_t.

#include "core/fxcrt/fx_system.h"
#include "third_party/base/numerics/safe_math.h"

using FX_SAFE_UINT32 = pdfium::base::CheckedNumeric<uint32_t>;
using FX_SAFE_INT32 = pdfium::base::CheckedNumeric<int32_t>;
using FX_SAFE_SIZE_T = pdfium::base::CheckedNumeric<size_t>;
using FX_SAFE_FILESIZE = pdfium::base::CheckedNumeric<FX_FILESIZE>;

#endif  // CORE_FXCRT_FX_SAFE_TYPES_H_
