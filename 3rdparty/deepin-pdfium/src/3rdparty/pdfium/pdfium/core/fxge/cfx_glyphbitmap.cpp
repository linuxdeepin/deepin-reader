// Copyright 2019 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/fxge/cfx_glyphbitmap.h"

#include "core/fxge/dib/cfx_dibitmap.h"

CFX_GlyphBitmap::CFX_GlyphBitmap(int left, int top)
    : m_Left(left), m_Top(top), m_pBitmap(pdfium::MakeRetain<CFX_DIBitmap>()) {}

CFX_GlyphBitmap::~CFX_GlyphBitmap() = default;
