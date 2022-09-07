// Copyright 2019 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CORE_FPDFAPI_EDIT_CPDF_CONTENTSTREAM_WRITE_UTILS_H_
#define CORE_FPDFAPI_EDIT_CPDF_CONTENTSTREAM_WRITE_UTILS_H_

#include <ostream>

#include "core/fxcrt/fx_coordinates.h"

std::ostream& WriteFloat(std::ostream& stream, float value);
std::ostream& operator<<(std::ostream& ar, const CFX_Matrix& matrix);
std::ostream& operator<<(std::ostream& ar, const CFX_PointF& point);

#endif  // CORE_FPDFAPI_EDIT_CPDF_CONTENTSTREAM_WRITE_UTILS_H_
