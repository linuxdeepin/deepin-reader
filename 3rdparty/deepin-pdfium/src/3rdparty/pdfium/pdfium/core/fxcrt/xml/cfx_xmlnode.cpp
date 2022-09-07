// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/xml/cfx_xmlnode.h"

CFX_XMLNode::CFX_XMLNode() = default;

CFX_XMLNode::~CFX_XMLNode() = default;

void CFX_XMLNode::InsertChildNode(CFX_XMLNode* pNode, int32_t index) {
  InsertBefore(pNode, GetNthChild(index));
}

CFX_XMLNode* CFX_XMLNode::GetRoot() {
  CFX_XMLNode* pParent = this;
  while (pParent->GetParent())
    pParent = pParent->GetParent();

  return pParent;
}
