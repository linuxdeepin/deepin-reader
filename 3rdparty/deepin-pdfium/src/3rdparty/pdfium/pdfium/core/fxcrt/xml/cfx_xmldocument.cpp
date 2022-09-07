// Copyright 2018 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/fxcrt/xml/cfx_xmldocument.h"

#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "core/fxcrt/xml/cfx_xmlinstruction.h"

CFX_XMLDocument::CFX_XMLDocument() {
  root_ = CreateNode<CFX_XMLElement>(L"root");
}

CFX_XMLDocument::~CFX_XMLDocument() = default;

void CFX_XMLDocument::AppendNodesFrom(CFX_XMLDocument* other) {
  nodes_.reserve(nodes_.size() + other->nodes_.size());
  nodes_.insert(nodes_.end(), std::make_move_iterator(other->nodes_.begin()),
                std::make_move_iterator(other->nodes_.end()));
  other->nodes_.clear();
}
